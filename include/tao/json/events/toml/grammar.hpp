// Copyright (c) 2017 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAOCPP_JSON_INCLUDE_EVENTS_TOML_GRAMMAR_HPP
#define TAOCPP_JSON_INCLUDE_EVENTS_TOML_GRAMMAR_HPP

#include "../../internal/grammar.hpp"

namespace tao
{
   namespace json
   {
      namespace events
      {
         namespace toml
         {
            // clang-format off
            namespace rules
            {
               using namespace json_pegtl;

               struct ws : star< blank > {};
               struct hash : one< '#' > {};
               struct single : one< '\'' > {};
               struct double : one< '"' > {};
               struct equals : one< '=' > {};
               struct under : one< '_' > {};
               struct sign : one< '-', '+' > {};
               struct triple_single : string< '\'', '\'', '\'' > {};
               struct triple_double : string< '"', '"', '"' > {};
               struct discarded : eol {};
               struct false_ : TAOCPP_JSON_PEGTL_STRING( "false" ) {};
               struct true_ : TAOCPP_JSON_PEGTL_STRING( "true" ) {};

               template< typename R, typename P = blank >
               using padr = json_pegtl::internal::seq< R, json_pegtl::internal::star< P > >;

               template< typename R, typename ... S >
               struct opt_must : if_must_else< R, seq< S ... >, success > {};  // TODO: Native PEGTL rule with dedicated implementation?

               struct non_eol : utf8::ranges< 0x20, 0x7e, 0x80, 0x10ffff, 0x09 > {};
               struct comment : if_must_else< hash, until< eolf, non_eol >, eolf > {};

               struct single_literal : if_must< single, until< at< single >, not_one< '\'', '\r', '\n' > >, single > {};
               struct multi_literal : if_must< triple_single, opt< discarded >, until< at< triple_single > >, triple_single > {};

               struct bare_char : one< '-', '_' > {};
               struct bare_key : plus< sor< alnum, bare_char > > {};
               struct key : sor< bare_key, single_string, single_literal > {};

               struct value;

               struct key_value : if_must< key, ws, equals, ws, value > {};

               struct single_string;  // TODO
               struct multi_string;  // TODO

               struct isign : sign {};
               struct idigit : digit {};
               struct integer : seq< opt< isign >, list_must< plus< idigit >, under > > {};

               struct inline_table : if_must< one< '{' >, ws, opt< list_must< key_value, one< ',' >, blank >, ws >, one< '}' > > {};
               struct array : if_must< one< '[' >, ws, 

               struct value : sor< multi_string, single_string, multi_literal, single_literal, true_, false_, integer, floating, array, inline_table > {};  // TODO: Add datetime when ready.





               struct begin_array : padr< one< '[' > > {};
               struct begin_object : padr< one< '{' > > {};
               struct end_array : one< ']' > {};
               struct end_object : one< '}' > {};
               struct name_separator : pad< one< ':' >, ws > {};
               struct value_separator : padr< one< ',' > > {};
               struct element_separator : padr< one< ',' > > {};


               struct digits : plus< abnf::DIGIT > {};

               struct esign : one< '-', '+' > {};

               struct edigits : digits {};
               struct fdigits : digits {};
               struct idigits : digits {};

               struct exp : seq< one< 'e', 'E' >, opt< esign >, must< edigits > > {};
               struct frac : if_must< one< '.' >, fdigits > {};

               template< bool NEG >
               struct number : seq< idigits, opt< frac >, opt< exp > > {};

               struct xdigit : abnf::HEXDIG {};
               struct escaped_unicode : list< seq< one< 'u' >, rep< 4, must< xdigit > > >, one< '\\' > > {};

               struct escaped_char : one< '"', '\\', '/', 'b', 'f', 'n', 'r', 't' > {};
               struct escaped : sor< escaped_char, escaped_unicode > {};

               struct unescaped
               {
                  using analyze_t = json_pegtl::analysis::generic< json_pegtl::analysis::rule_type::ANY >;

                  template< typename Input >
                  static bool match( Input& in )
                  {
                     bool result = false;

                     while( !in.empty() ) {
                        if( const auto t = json_pegtl::internal::peek_utf8::peek( in ) ) {
                           if( ( 0x20 <= t.data ) && ( t.data <= 0x10ffff ) && ( t.data != '\\' ) && ( t.data != '"' ) ) {
                              in.bump_in_this_line( t.size );
                              result = true;
                              continue;
                           }
                        }
                        return result;
                     }
                     throw json_pegtl::parse_error( "invalid character in string", in );
                  }
               };

               struct chars : if_then_else< one< '\\' >, must< escaped >, unescaped > {};

               struct string_content : until< at< one< '"' > >, must< chars > > {};
               struct string : seq< one< '"' >, must< string_content >, any >
               {
                  using content = string_content;
               };

               struct key_content : string_content {};
               struct key : seq< one< '"' >, must< key_content >, any >
               {
                  using content = key_content;
               };

               struct value;

               struct array_element;
               struct array_content : opt< list_must< array_element, element_separator > > {};
               struct array : seq< begin_array, array_content, must< end_array > >
               {
                  using begin = begin_array;
                  using end = end_array;
                  using element = array_element;
                  using content = array_content;
               };

               struct member : if_must< key, name_separator, value > {};
               struct object_content : opt< list_must< member, value_separator > > {};
               struct object : seq< begin_object, object_content, must< end_object > >
               {
                  using begin = begin_object;
                  using end = end_object;
                  using element = member;
                  using content = object_content;
               };

               template< bool NEG >
               struct zero {};

               struct sor_value
               {
                  using analyze_t = json_pegtl::analysis::generic< json_pegtl::analysis::rule_type::SOR, string, number< false >, object, array, false_, true_, null >;

                  template< bool NEG,
                            apply_mode A,
                            rewind_mode M,
                            template< typename... > class Action,
                            template< typename... > class Control,
                            typename Input,
                            typename... States >
                  static bool match_zero( Input& in, States&&... st )
                  {
                     if( in.size( 2 ) > 1 ) {
                        switch( in.peek_char( 1 ) ) {
                           case '.':
                           case 'e':
                           case 'E':
                              return Control< number< NEG > >::template match< A, M, Action, Control >( in, st... );

                           case '0':
                           case '1':
                           case '2':
                           case '3':
                           case '4':
                           case '5':
                           case '6':
                           case '7':
                           case '8':
                           case '9':
                              throw json_pegtl::parse_error( "invalid leading zero", in );
                        }
                     }
                     in.bump_in_this_line();
                     Control< zero< NEG > >::template apply0< Action >( in, st... );
                     return true;
                  }

                  template< bool NEG,
                            apply_mode A,
                            rewind_mode M,
                            template< typename... > class Action,
                            template< typename... > class Control,
                            typename Input,
                            typename... States >
                  static bool match_number( Input& in, States&&... st )
                  {
                     if( in.peek_char() == '0' ) {
                        if( !match_zero< NEG, A, rewind_mode::DONTCARE, Action, Control >( in, st... ) ) {
                           throw json_pegtl::parse_error( "incomplete number", in );
                        }
                        return true;
                     }
                     else {
                        return Control< number< NEG > >::template match< A, M, Action, Control >( in, st... );
                     }
                  }

                  template< apply_mode A,
                            rewind_mode M,
                            template< typename... > class Action,
                            template< typename... > class Control,
                            typename Input,
                            typename... States >
                  static bool match_impl( Input& in, States&&... st )
                  {
                     switch( in.peek_char() ) {
                        case '"': return Control< string >::template match< A, M, Action, Control >( in, st... );
                        case '{': return Control< object >::template match< A, M, Action, Control >( in, st... );
                        case '[': return Control< array >::template match< A, M, Action, Control >( in, st... );
                        case 'n': return Control< null >::template match< A, M, Action, Control >( in, st... );
                        case 't': return Control< true_ >::template match< A, M, Action, Control >( in, st... );
                        case 'f': return Control< false_ >::template match< A, M, Action, Control >( in, st... );

                        case '-':
                           in.bump_in_this_line();
                           if( in.empty() || !match_number< true, A, rewind_mode::DONTCARE, Action, Control >( in, st... ) ) {
                              throw json_pegtl::parse_error( "incomplete number", in );
                           }
                           return true;

                        default:
                           return match_number< false, A, M, Action, Control >( in, st... );
                     }
                  }

                  template< apply_mode A,
                            rewind_mode M,
                            template< typename... > class Action,
                            template< typename... > class Control,
                            typename Input,
                            typename... States >
                  static bool match( Input& in, States&&... st )
                  {
                     if( in.size( 2 ) && match_impl< A, M, Action, Control >( in, st... ) ) {
                        in.discard();
                        return true;
                     }
                     return false;
                  }
               };

               struct value : padr< sor_value > {};
               struct array_element : value {};

               struct text : seq< star< ws >, value > {};

            }  // namespace rules

            struct grammar : json_pegtl::must< rules::text, json_pegtl::eof > {};
            // clang-format on

         }  // namespace toml

      }  // namespace events

   }  // namespace json

}  // namespace tao

#endif
