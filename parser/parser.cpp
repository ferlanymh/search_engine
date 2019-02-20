//数据解析模块框架：
//1. 将所有html路径合并到一个行文本文件，文件中每一行都是boost库文档的一个html文件路径。
//2. 挨个对行文本文件中每个html路径进行解析，提取出各个html文件的标题title，正文content(重点是去除标签)，跳转URL
//3. 将每个文件的分析结果以一行的形式再写入到另一个输出文件output中
//

#include<iostream>
#include<string>
#include<vector>
#include<boost/filesystem/path.hpp>//会使用到boost库，也要引入一下
#include<boost/filesystem/operations.hpp>
#include"../common/util.hpp"//包含我们设计的工具类


const std::string input_path = "../data/input/html/";//输入文件路径
const std::string output_path = "../data/tmp/output";//输出文件路径


//到第二步进行解析的时候，存储各个html文件的各项信息。最终在写入到输出文件output。
struct DocInfo
{
  std::string title;
  std::string content;
  std::string url;
};

bool EnumFile(const std::string input_path,std::vector<std::string>* file_list)//将html路径放进file_list
{
  //根据boost库中文档我们发现，input_path路径下不止有.html文件，还有很多其他的目录文件和.png文件。
  //我们要将需要的.html文件加入到file_list中，而把不要的过滤掉。
  
  //但是input_path路径下有很多的.html文件，而且目录文件中可能也包含我们要的文件，所以我们需要递归的遍历这个路径下  //的所有文件，以确保能将所有的.html加入到file_list中，我这里采用boost库中提供的递归遍历目录的方式。
  
  namespace fs = boost::filesystem;
  //input_path是一个字符串，我们利用它构造一个path对象
  fs::path root_path(input_path);
  if (!fs::exists(root_path)) //说明构建失败
  {
    std::cout << "input_path is not exist! input_path = " << input_path << std::endl;
    return false;
  }

  //现在开始遍历，boost递归遍历目录需要一个特殊的迭代器
  fs::recursive_directory_iterator end_iter;
  for (fs::recursive_directory_iterator iter(root_path);iter != end_iter;iter++)
  {
    //剔除目录和png等其他文件
    if (!fs::is_regular_file(*iter))//目录
      continue;
    if (iter->path().extension() != ".html")//不是.html后缀
      continue;

    //排除错误选项后就可以插入file_list了。
    file_list->push_back(iter->path().string());   

  }
  return true;
}


bool ParseTitle(const std::string& html,std::string* title)//解析标题
{
  //html标题是以<title>开头，</title>结尾，所以大致思路是:
  //1.找到<title>
  //2.找到</title>
  //3.通过字符串取子串的方式获取标题内容
  
  size_t begin = html.find("<title>");//查找<title>
  if (begin == std::string::npos)
  {
    std::cout<<"<title> not found!"<<std::endl;
    return false;
  }

  size_t end = html.find("</title>");//查找</title>
  if (end == std::string::npos)
  {
    std::cout<<"</title> not found!"<<std::endl;
    return false;
  }

  begin += std::string("<title>").size();
  if (begin > end)//标题可以为空，但begin的位置不应该在end之后
  
  {
    std::cout<<"begin position is wrong"<<std::endl;
    return false;
  }

  *title = html.substr(begin,end - begin);//利用substr分离出title 
  return true;
}

bool ParseContent(const std::string& html,std::string* content)
{
  //我们认为只要是html的内容都是正文，前提是我们得去掉标签，标签可不是正文的内容。
  //去掉标签的思路是：
  //1.从文件头开始时读，读到'<'就认为是标签开始，接下来读到的字符舍弃，
  //  直到读到'>'认为标签结束，正常接收后面的字符。
  //2.遇到正常的字符则直接写入content中。
  
  bool is_content = true;
  for (auto c : html)//使用基于范围的for循环
  {
    if (is_content)
    {
      if (c == '<')//标签开始
      {
        is_content = false;
        continue;
      }
      else 
      {
        if (c=='\n')
        {
          c = ' ';
        }
        content->push_back(c);
      }
    }
    else //说明现在读到的还是标签的内容，进行'>'的检查，不满足条件则丢弃
    {
      if (c == '>')//标签结束
        is_content = true;  
    }
  }
  return true;
}

bool ParseURL(const std::string& file_path, std::string* url)
{
  //我们发现每个html文件的URL都是有一定规律的。
  //每个URL的前半部分都是一样的。
  //而URL的后半部分则在file_path当中，所以我要做的就是前半部分和后半部分的拼接
  std::string first_part = "https://www.boost.org/doc/libs/1_69_0/doc/";
  //file_path路径：     ../data/input/html/array.html 
  //我们需要的是        /html/array.html
  
  std::string second_part = file_path.substr(input_path.size());//我们发现不要的部分刚好就是输入路径input_path
  *url = first_part + second_part;

  return true;
}


//解析传入的html路径，打开对应文件，将文件中的各项信息提取并加入到doc_info中
bool ParseFile(const std::string& file_path,DocInfo* doc_info)
{
  //1.先要将file_path路径的文件打开,将文件内容写入到html字符串中
  std::string html;
  bool ret = FileUtil::Read(file_path,&html);
  if (!ret)//读取失败
    std::cout<<"read file failed! the file_path is "<<file_path<<std::endl;
 

  //2.开始各项内容的解析
  ret = ParseTitle(html,&doc_info->title);//解析标题
  if (!ret)//解析标题失败
  {
    std::cout<<"ParseTitle failed!"<<std::endl;
    return false;
  }

  ret = ParseContent(html,&doc_info->content);//解析正文
  if (!ret)//解析正文失败
  {
    std::cout<<"ParseContent failed"<<std::endl;
    return false;
  }

  ret = ParseURL(file_path,&doc_info->url);//解析URL,这里因为file_path中包含我们最终结果的一部分，故传它
  if (!ret)//解析URL失败
  {
    std::cout<<"ParseURL failed"<<std::endl;
  }

  return true;
}

int main()
{
  std::vector<std::string> file_list;//用于存储html列表
  //1. 将所有html路径合并到一个行文本文件，文件中每一行都是boost库文档的一个html文件路径。
  bool ret = EnumFile(input_path,&file_list);
  if (!ret)//提取html路径失败
  {
    std::cout<<"EnumFile error!"<<std::endl;
    return 1;
  }
  for (const auto& str : file_list)
  {
    std::cout<<str<<std::endl;
  }

  


  //2. 挨个对行文本文件中每个html路径进行解析，提取出各个html文件的\
  //标题title，正文content(重点是去除标签)，跳转URL
  for (const auto& file_path : file_list)//这里用nclude <boost/filesystem.hpp>const auto&，免得深拷贝
  {
    DocInfo doc_info;
    ret = ParseFile(file_path,&doc_info);
    if (!ret)//文件解析失败，这里不应该直接终止程序，使用continue
    {
      std::cout<<"ParseFile failed! the filepath is "<<file_path<<std::endl;
      continue;
    }


    //3. 将每个文件的分析结果以一行的形式再写入到另一个输出文件output中
    WriteOutput(doc_info,output_file);    
  }
  
  return 0;
}


