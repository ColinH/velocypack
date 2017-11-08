// Copyright (c) 2017 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAOCPP_JSON_INCLUDE_EVENTS_VIRTUAL_BASE_HPP
#define TAOCPP_JSON_INCLUDE_EVENTS_VIRTUAL_BASE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "../byte.hpp"

namespace tao
{
   namespace json
   {
      namespace events
      {
         // Events consumer with virtual functions.

         struct virtual_base
         {
            virtual_base( const virtual_base& ) = delete;
            void operator= ( const virtual_base& ) = delete;

            virtual void null() = 0;
            virtual void boolean( const bool ) = 0;
            virtual void number( const std::int64_t ) = 0;
            virtual void number( const std::uint64_t ) = 0;
            virtual void number( const double ) = 0;
            virtual void string( const std::string& ) = 0;
            virtual void binary( const std::vector< byte >& ) = 0;
            virtual void begin_array() = 0;
            virtual void begin_array( const std::size_t ) = 0;
            virtual void element() = 0;
            virtual void end_array() = 0;
            virtual void end_array( const std::size_t ) = 0;
            virtual void begin_object() = 0;
            virtual void begin_object( const std::size_t ) = 0;
            virtual void key( const std::string& ) = 0;
            virtual void member() = 0;
            virtual void end_object() = 0;
            virtual void end_object( const std::size_t ) = 0;

         protected:
            ~virtual_base()
            {
               // TODO: protected OR public+virtual?
            }
         };

      }  // namespace events

   }  // namespace json

}  // namespace tao

#endif
