#ifndef _MARKDOWN_PARSER_H_
#define _MARKDOWN_PARSER_H_

#include "vasdef.hpp"

namespace vas {
namespace mkd {

enum TOKEN : u8
{
  title,         // #
  link,          // [text](url)
  image,         // ![alt](src)
  codemark,      // `, maybe part of codeblock or text
  codeblock,     // ```
  list,          // *
  quote,         // >
  hr,            // ---
  table,         // | header |
  bold,          // **
  italic,        // *
  strikethrough, // ~~
  newline,       // \n\n
  text,          // plain text
  space,         // ' ' or '\t' or '\n'
  eof,           // end of file
};

struct token_t
{
  TOKEN type;
  string_view content;
  u32 line;
  u32 column;
};

class Parser
{
  vector<token_t> tokens;
  // todo
public:
  Parser( const string& filepath );
};

}
}

#endif