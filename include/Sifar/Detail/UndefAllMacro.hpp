// If you want to undef all external lib's macros
// you should include given file only after library files
// note that: MACRO - is public macro, _MACRO - is private

// clean up
#undef SERIALIZABLE

#undef SERIALIZATION_SAVE_DATA
#undef SERIALIZATION_LOAD_DATA
#undef SERIALIZATION_SAVE_LOAD_DATA

#undef SERIALIZATION_TYPE_REGISTRY
#undef SERIALIZATION_CONDITIONAL_TYPE_REGISTRY

#undef SERIALIZATION_ARCHIVE
#undef _EXPORT_SERIALIZATION_ARCHIVE_TRAIT_KEY
#undef _EXPORT_SERIALIZATION_ARCHIVE_TYPE_TRAIT
#undef EXPORT_SERIALIZATION_ARCHIVE

#undef _CLONEABLE_KEY_CALL
#undef _CLONEABLE_KEY_IMPLEMENTATION
#undef _CLONEABLE_FACTORY_TABLE_IMPLEMENTATION
#undef _CLONEABLE_IMPLEMENTATION
#undef CLONEABLE_BODY

#undef EXPORT_POLYMORPHIC_KEY
#undef EXPORT_POLYMORPHIC
#undef _POLYMORPHIC_ARCHIVE_CALL
#undef _DYNAMIC_SAVE_LOAD_IMPLEMENTATION
#undef _POLYMORPHIC_BASE_BODY
#undef POLYMORPHIC_BASE
#undef SERIALIZABLE_POLYMORPHIC
#undef POLYMORPHIC

#undef _SERIALIZATION_DECLARATION_IF
#undef _SERIALIZATION_DECLARATION
#undef _SERIALIZATION_IMPLEMENTATION_IF
#undef _SERIALIZATION_IMPLEMENTATION
#undef CONDITIONAL_LOAD_SERIALIZABLE_TYPE
#undef LOAD_SERIALIZABLE_TYPE
#undef CONDITIONAL_SAVE_SERIALIZABLE_TYPE
#undef SAVE_SERIALIZABLE_TYPE
#undef CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE
#undef SAVE_LOAD_SERIALIZABLE_TYPE
#undef CONDITIONAL_SAVE_SERIALIZABLE
#undef CONDITIONAL_LOAD_SERIALIZABLE
#undef _SERIALIZATION_CALL
#undef CONDITIONAL_SAVE_LOAD_SERIALIZABLE
#undef SAVE_SERIALIZABLE
#undef LOAD_SERIALIZABLE
#undef SAVE_LOAD_SERIALIZABLE
#undef CONDITIONAL_SERIALIZATION
#undef SERIALIZATION

#undef SIFAR_STATIC_HASH
#undef SIFAR_STATIC_HASH_KEY_TYPE
#undef SIFAR_SMART

#undef SIREQUIRE
#undef SIWHEN
#undef SIWITH
