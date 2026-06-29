# Style Guidelines

## Language and Tooling
- Python may be used for generating test data or plots, as long as it only uses the python standard library.
- Write all other code in modern modules c++ available in gcc 16.0.1.
- We do not have the correct compile flags to use import statements for the standard library, but no worries, we can use the regular #include for them.

## Module File Structure
- We do not use separate header and implementation files: the entire module goes in a .cpp file.
- Start each file with "module;", then #includes, then "export module {module name};".
- Each .cpp file should export one and only class/namespace/struct/thingy that should have the exact same name as the module AND the file name.

## Namespace and Folder Structure
- The folder structure mirrors the module/namespace structure.
- Everything in a directory is exported through a namespace matching that directory name (e.g., everything in `common/` is in `namespace common`, everything in `exchange/` will be in `namespace exchange`).
- Macros cannot be part of modules, so we use "manual namespacing": prefix macro names with the uppercase namespace and `_MACRO_` (e.g., `COMMON_MACRO_DELETE_CONSTRUCTOR`). Avoid macros when possible.

## Naming
- We use pascal case for types, and camel case for everything else.
- Do not use abbreviations nor acronyms, write out entire words, except for when the abbreviation is more common than the full word such as:
	- "id" for identifier
	- "max" and "min" for maximum and minimum
	or similar.

## Syntax
- Always access class member variables using the explicit `this->` pointer (e.g., `this->pos` instead of `pos`, `this->body_buffer` instead of `body_buffer`).
- Avoid empty lines. Definitively do not put an empty line between functions or methods. Exceptions:
	- One empty line between constructor implementation and the first method of a class.

## Flexibility
- These rules are the target for completed, polished code. During active development — especially when adapting examples from the book — flexibility is expected. We need to understand how things are used before renaming or restructuring, so we can verify our results against the book.
