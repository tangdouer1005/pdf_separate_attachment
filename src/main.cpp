#include "pdf_hummus_make_attachment.hpp"

#include "string.hpp"
#include <getopt.h>
#include <iostream>
int main(int argc, char *argv[]) {
  string project_path = string(getenv("PROJECT_PATH")) * "/";
  string pdf_path = project_path * string("default.pdf");

  list<string> attachment_paths = list<string>();
  attachment_paths =
      attachment_paths * string(project_path * string("default_1.tm"));
  attachment_paths =
      attachment_paths * string(project_path * string("default_2.tm"));
  attachment_paths =
      attachment_paths * string(project_path * string("default_3.tm"));

  string out_path = project_path * string("default_out.pdf");
  // int c = 0;
  // while(EOF != (c = getopt(argc,argv,"t:p:o:")))
  // {
  //     switch(c)
  //     {
  //         case 't':
  //             std::cout << "tmPath " << optarg << std::endl;
  //             attachment_path = project_path * string(optarg);
  //             break;
  //         case 'p':
  //             std::cout << "pdfPath " << optarg << std::endl;
  //             pdf_path = project_path* string(optarg);
  //             break;
  //         case 'o':
  //             std::cout << "outPdfPath " << optarg << std::endl;
  //             out_path = project_path* string(optarg);
  //             break;
  //         default:
  //             std::cout << "bad arg" << std::endl;
  //             exit(0);
  //             break;
  //     }
  // }
  EStatusCode status;
  status = pdf_hummus_make_attachment(pdf_path, attachment_paths, out_path);
  if (status != eSuccess) {
    cout << "prog fail\n";
  } else {
    cout << "prog success\n";
  }
}