#include<iostream>
#include"./httplib.h"
#include"./searcher.h"
int main()
{
  searcher::Searcher s;
  s.init("../data/tmp/output");
  
  using namespace httplib;
  Server server;
  server.Get("/searcher",[&s](const Request& req, Response& res) {//   /search表示根目录
      std::cout<<req.path<<std::endl;
      std::string query = req.get_param_value("query");
      std::string result;
     
      s.search(query,&result);
      res.set_content(result,"text/plain");
  });

  server.listen("0.0.0.0",9092);
  return 0;
}
