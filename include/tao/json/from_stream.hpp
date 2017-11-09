// Copyright (c) 2015-2017 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAOCPP_JSON_INCLUDE_FROM_STREAM_HPP
#define TAOCPP_JSON_INCLUDE_FROM_STREAM_HPP

#include <cstddef>
#include <string>
#include <utility>

#include "events/from_stream.hpp"
#include "events/to_value.hpp"
#include "events/transformer.hpp"

namespace tao
{
   namespace json
   {
      template< template< typename... > class Traits, template< typename... > class... Transformers >
      basic_value< Traits > basic_from_stream( std::istream& stream, const char* source = nullptr, const std::size_t maximum_buffer_size = 4000 )
      {
         events::transformer< events::to_basic_value< Traits >, Transformers... > consumer;
         events::from_stream( consumer, stream, source, maximum_buffer_size );
         return std::move( consumer.value );
      }

      template< template< typename... > class Traits, template< typename... > class... Transformers >
      basic_value< Traits > basic_from_stream( std::istream& stream, const std::string& source, const std::size_t maximum_buffer_size = 4000 )
      {
         return basic_from_stream< Traits, Transformers... >( stream, source.c_str(), maximum_buffer_size );
      }

      template< template< typename... > class... Transformers >
      value from_stream( std::istream& stream, const char* source, const std::size_t maximum_buffer_size = 4000 )
      {
         return basic_from_stream< traits, Transformers... >( stream, source, maximum_buffer_size );
      }

      template< template< typename... > class... Transformers >
      value from_stream( std::istream& stream, const std::string& source, const std::size_t maximum_buffer_size = 4000 )
      {
         return basic_from_stream< traits, Transformers... >( stream, source.c_str(), maximum_buffer_size );
      }

   }  // namespace json

}  // namespace tao

#endif
