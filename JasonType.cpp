
#include "JasonType.h"

using JasonType = triagens::basics::JasonType;

////////////////////////////////////////////////////////////////////////////////
/// @brief get the name for a Jason type
////////////////////////////////////////////////////////////////////////////////
    
char const* triagens::basics::JasonTypeName (JasonType type) {
  switch (type) {
    case JasonType::None:        return "none";
    case JasonType::Null:        return "null";
    case JasonType::Bool:        return "bool";
    case JasonType::Double:      return "double";
    case JasonType::String:      return "string";
    case JasonType::Array:       return "array";
    case JasonType::Object:      return "object";
    case JasonType::External:    return "external";
    case JasonType::ID:          return "id";
    case JasonType::ArangoDB_id: return "arangodb_id";
    case JasonType::UTCDate:     return "utc-date";
    case JasonType::Int:         return "int";
    case JasonType::UInt:        return "uint";
    case JasonType::Binary:      return "binary";
  }

  return "unknown";
}

