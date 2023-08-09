#include "PDFWriter/PDFParser.h"
#include "PDFWriter/InputFile.h"
#include "PDFWriter/PDFObject.h"
#include "PDFWriter/PDFDictionary.h"
#include "PDFWriter/PDFObjectCast.h"
#include "PDFWriter/PDFIndirectObjectReference.h"
#include "PDFWriter/PDFArray.h"
#include "PDFWriter/PDFDictionary.h"
#include "PDFWriter/PDFStreamInput.h"
#include "PDFWriter/OutputFile.h"
#include "PDFWriter/OutputStreamTraits.h"
#include "PDFWriter/OutputFileStream.h"

#include "PDFWriter/IByteWriterWithPosition.h"
#include "PDFWriter/PrimitiveObjectsWriter.h"
#include "PDFWriter/ObjectsBasicTypes.h"
#include "PDFWriter/PDFLiteralString.h"


#include <iostream>
#include <set>

using namespace std;
using namespace PDFHummus;
int main(int argc, char *argv[])
{
  string project_path = string(getenv("PROJECT_PATH"));
  EStatusCode status = PDFHummus::eSuccess;
  InputFile pdfFile;
  PDFParser parser;
  do
  {
    status = pdfFile.OpenFile(project_path + "/parser.pdf");
    if(status != PDFHummus::eSuccess)
		{
			cout<<"fail to open pdf\n";
			break;
		}
    parser.StartPDFParsing(pdfFile.GetInputStream());
    PDFObjectCastPtr<PDFDictionary> catalog(parser.QueryDictionaryObject(parser.GetTrailer(), "Root"));
    //return 0;
    if (!catalog)
    {
      cout << "Can't find catalog. fail\n";
      status = PDFHummus::eFailure;
      break;
    }

    PDFObjectCastPtr<PDFDictionary> d_1(catalog->QueryDirectObject("Names"));
    if (!d_1)
    {
      cout << "Can't find d1. fail\n";
      status = PDFHummus::eFailure;
      break;
    }
    PDFObjectCastPtr<PDFDictionary> d_2(d_1->QueryDirectObject("EmbeddedFiles"));
    if (!d_2)
    {
      cout << "Can't find d2. fail\n";
      status = PDFHummus::eFailure;
      break;
    }

    PDFObjectCastPtr<PDFArray> arr(d_2->QueryDirectObject("Names"));
    if (!arr)
    {
      cout << "Can't find arr. fail\n";
      status = PDFHummus::eFailure;
      break;
    }
    unsigned long n = arr->GetLength();
    cout << n;
    if (n & 1)
    {
      cout << "n is wrong\n";
      break;
    }
    cout << " here\n" << endl;
    for (unsigned long i = 0; i < n; i += 2)
    {
      PDFObjectCastPtr<PDFLiteralString> name(arr->QueryObject(i));
      if (!name)
      {
        cout << "Can't find name\n";
        status = PDFHummus::eFailure;
        break;
      }
      PDFObjectCastPtr<PDFDictionary> arr_d1(arr->QueryObject(i + 1));
      if (!arr_d1)
      {
        cout << "Can't find arr_d1\n";
        status = PDFHummus::eFailure;
        break;
      }
      PDFObjectCastPtr<PDFDictionary> arr_d2(arr_d1->QueryDirectObject("EF"));
      if (!arr_d2)
      {
        cout << "Can't find arr_d2\n";
        status = PDFHummus::eFailure;
        break;
      }
      PDFObjectCastPtr<PDFStreamInput> stream(parser.QueryDictionaryObject(arr_d2.GetPtr(), "F"));
      if (!stream)
      {
        cout << "Can't find stream\n";
        status = PDFHummus::eFailure;
        break;
      }
      OutputFileStream *out_stream = new OutputFileStream(project_path + string("/")+name -> GetValue());

      OutputStreamTraits copy(out_stream);
      
      copy.CopyToOutputStream(parser.StartReadingFromStream(stream.GetPtr()));
      out_stream -> Close();
    }
    if(status == PDFHummus::eFailure)
      break;

  } while (0);
}