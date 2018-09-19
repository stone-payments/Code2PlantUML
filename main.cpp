#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
// https://developer.mozilla.org/en-US/docs/Mozilla/Developer_guide/Coding_Style
class UMLClass
{
private:
  std::string m_name;
  std::vector<std::string> m_parent;
  bool m_isChild;
  std::list<std::pair<std::string, std::string>> m_members;

public:
  void setName(const std::string& name) { m_name = name; }
  void setParent(const std::vector<std::string>& parent) { m_parent = parent; }
  void setIsChild(const bool isChild) { m_isChild = isChild; }
  void setMember(const std::string& access, const std::string& member)
  {
    m_members.push_back(std::make_pair(access, member));
  }
  std::string text()
  {
    std::string result = "";
    result += m_name + '\n';
    for (auto itor = m_members.begin(); itor != m_members.end(); itor++) {
      result += "   " + itor->first;
      result += "   " + itor->second + '\n';
    }
    return result;
  }
  void uml(std::ofstream& ofs)
  {
    // - private
    // # protected
    // ~ package private
    // + public
    int index = 0;
    if (m_name.empty())
      return;
    // ofs.open(m_name + ".puml");
    // ofs << "@startuml\n";
    if (m_isChild) {
      for (auto itor = m_parent.begin(); itor != m_parent.end(); itor++) {
        ofs << m_name << " <|-- " << *itor << "\n";
      }
    }
    ofs << "class " << m_name << " {\n";

    // ofs << " -field1\n";
    // ofs << " #field2\n";
    // ofs << " ~method1()\n";
    // ofs << " +method2()\n";
    for (auto itor = m_members.begin(); itor != m_members.end(); itor++) {
      std::string result;
      if (itor->first.compare("public") == 0)
        result += " +";
      else if (itor->first.compare("protected") == 0)
        result += " #";
      else if (itor->first.compare("private") == 0)
        result += " -";
      else
        result += " ?";
      result += itor->second + '\n';
      ofs << result;
    }
    ofs << "}\n";
    // ofs << "@enduml\n";
    // ofs.close();
  }
  void dot()
  {
    std::ofstream ofs;
    ofs.open("mamba.dot");
    ofs << "digraph hierarchy {\n";
    ofs << "size=\"5,5\"\n";
    ofs << "node[shape=record,style=filled,fillcolor=gray95]\n";
    ofs << "edge[dir=back, arrowtail=empty]\n";
    ofs << "2[label = \"{Class AbstractSuffixTree|+ text\n+ root|...}\"]\n";
    ofs << "3[label = \"{SimpleSuffixTree|...| + constructTree()\\l...}\"]\n";
    ofs << "4[label = \"{CompactSuffixTree|...| + compactNodes()\\l...}\"]\n";
    ofs << "5[label = \"{SuffixTreeNode|...|+ addSuffix(...)\\l...}\"]\n";
    ofs << "6[label = \"{SuffixTreeEdge|...|+ compactLabel(...)\\l...}\"]\n";
    ofs << "2->3\n";
    ofs << "2->4\n";
    ofs << "5->5[constraint=false, arrowtail=odiamond]\n";
    ofs << "4->3[constraint=false, arrowtail=odiamond]\n";
    ofs << "2->5[constraint=false, arrowtail=odiamond]\n";
    ofs << "5->6[arrowtail=odiamond]\n";
    ofs << "}\n";
    ofs.close();
  }
};

std::map<std::string, UMLClass> classes;

void
insert_class(const std::string& name,
             const std::string& access,
             const std::string& member)
{
  classes[name].setName(name);
  classes[name].setMember(access, member);
  //  classes[name].m_members.insert(std::make_pair(access, member));
}
class line
{
  std::string data;

public:
  friend std::istream& operator>>(std::istream& is, line& l)
  {
    std::getline(is, l.data);
    return is;
  }
  operator std::string() const { return data; }
};
struct line_reader : std::ctype<char>
{
  line_reader()
    : std::ctype<char>(get_table())
  {}
  static std::ctype_base::mask const* get_table()
  {
    static std::vector<std::ctype_base::mask> rc(table_size,
                                                 std::ctype_base::mask());
    rc[' '] = std::ctype_base::space;
    rc[';'] = std::ctype_base::space;
    rc['"'] = std::ctype_base::space;
    rc['\t'] = std::ctype_base::space;
    rc[','] = std::ctype_base::space;
    rc['\n'] = std::ctype_base::alpha;
    return &rc[0];
  }
};
class commaDelimiter : public std::string
{};
class Tag
{
private:
  std::string name;
  std::string file;
  std::string address;
  std::string type;

public:
  Tag()
    : name("")
    , file("")
    , address("")
    , type("")

  {}
  void setName(const std::string& _name) { name = _name; }
  void setFile(const std::string& _file) { file = _file; }
  void setAddress(const std::string& _address) { address = _address; }
  void setType(const std::string& _type) { type = _type; }
  const std::string getName() { return name; }
  const std::string getFile() { return file; }
  const std::string getAddress() { return address; }
  const std::string getType() { return type; }
  bool typeCompare(const std::string& _type)
  {
    return type.compare(_type) == 0;
  }
  friend std::ostream& operator<<(std::ostream& os, Tag const& t)
  {
    return os << t.name << "\t" << t.file << "\t" << t.address << "\t"
              << t.type;
  }
};

bool
IsColon(char c)
{
  return (c == ':');
}

void
processTags(const std::string& line)
{
  Tag tag;
  std::map<std::string, std::string> field;
  std::vector<std::string> lines;
  std::istringstream iss(line);

  iss.imbue(std::locale(std::locale(), new line_reader()));

  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::inserter(lines, lines.end()));
  /*
    //debug...
    std::copy(lines.begin(),
            lines.end(),
            std::ostream_iterator<std::string>(std::cout, "\t"));*/
  if (lines.size() >= 4) {

    auto itor = lines.begin();
    tag.setName(*itor);
    ++itor;
    tag.setFile(*itor);
    ++itor;
    tag.setAddress(*itor);
    ++itor;
    tag.setType(*itor);
    if (0) { // debug
      std::cout << tag << "\t";
    }
    ++itor;
    // parse fields:
    for (; itor != lines.end(); itor++) {
      size_t pos = itor->find(':');
      std::string fieldName = itor->substr(0, pos);
      std::string fieldValue = itor->substr(pos + 1, itor->size() - pos);

      // change :: to __ due to problem in plantuml parsing...
      std::replace_if(fieldValue.begin(), fieldValue.end(), IsColon, '_');
      if (0) { // debug
        std::cout << "[" << fieldName << "|" << fieldValue << "]";
      }
      field.insert(std::make_pair(fieldName, fieldValue));
    }
    std::cout << std::endl;
  }
  // analise data!
  bool isClass = false;
  bool isMember = false;
  bool isMemberFunction = false;
  bool isChild = false;
  std::string className = "global_namespace";
  std::string access = "public";
  std::vector<std::string> parent;

  if (tag.typeCompare("c")) {
    isClass = true;
    className = tag.getName();
  }
  if (tag.typeCompare("m")) {
    isMember = true;
  }
  if (tag.typeCompare("p")) {
    isMemberFunction = true;
  }
  for (auto itor = field.begin(); itor != field.end(); itor++) {
    if (itor->first.compare("namespace") == 0) {
      className = itor->second + "__";
    }
    if (itor->first.compare("struct") == 0) {
      className = itor->second;
    }
    if (itor->first.compare("class") == 0) {
      className = itor->second;
    }
    if (itor->first.compare("access") == 0) {
      access = itor->second;
    }
    if (itor->first.compare("inherits") == 0) {
      // QObject,CThread
      std::istringstream iss(itor->second);
      std::vector<std::string> result(
        (std::istream_iterator<commaDelimiter>(iss)),
        std::istream_iterator<commaDelimiter>());
      std::copy(result.begin(), result.end(), std::back_inserter(parent));
      isChild = true;
    }
  }

  if (isClass && isChild) {
    classes[className].setParent(parent);
    classes[className].setIsChild(isChild);
  }
  if (isMemberFunction) {
    std::string tmp = tag.getName();
    tag.setName(tmp + "()");
    insert_class(className, access, tag.getName());
  } else if (isMember) {
    insert_class(className, access, tag.getName());
  }
  if (0) { // debug
    std::cout << className << "_" << access << "|" << tag.getName() << "|"
              << std::endl;
  }
}
void
global_uml_head(std::ofstream& ofs)
{
  ofs << "@startuml\n";
}
void
global_uml_footer(std::ofstream& ofs)
{
  ofs << "@enduml\n";
  ofs.close();
}
int
main(int argc, char** argv)
{
  int index = 0;
  if (argc < 2) {
    std::cout << "need to pass the file name!" << std::endl;
  }

  std::ifstream ifs(std::string(argv[1]) + "/tags");
  for (std::string line; std::getline(ifs, line);) {
    // skip file information - 6 first lines
    if (index < 6) {
      ++index;
      std::cout << line << std::endl;
    } else
      processTags(line);
  }
  ifs.close();
  std::ofstream puml_file;
  puml_file.open(std::string(argv[1]) + ".puml");
  global_uml_head(puml_file);
  for (auto itor = classes.begin(); itor != classes.end(); itor++) {
    // std::cout << itor->second.text() << std::endl;
    // itor->second.dot();
    itor->second.uml(puml_file);
  }
  global_uml_footer(puml_file);

  return 0;
}
