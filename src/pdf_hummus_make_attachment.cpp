
#include "pdf_hummus_make_attachment.hpp"

class PDFAttachment {
public:
  PDFAttachment(void);
  PDFAttachment(string attachment_path);
  PDFAttachment(Byte inByte[], size_t inLenth, string inName);
  ~PDFAttachment(void);

  Byte *file_content;
  size_t lenth;
  string name;
};

class PDFWriter;

typedef hashmap<PDFAttachment *, ObjectIDType> PDFAttachmentToObjectIDTypeMap;

class PDFAttachmentWriter : public DocumentContextExtenderAdapter {
public:
  PDFAttachmentWriter(PDFWriter *inPDFWriter);
  ~PDFAttachmentWriter(void);

  EStatusCode AttachToAllPage(PDFAttachment *inAttachment);

  virtual EStatusCode
  OnCatalogWrite(CatalogInformation *inCatalogInformation,
                 DictionaryContext *inCatalogDictionaryContext,
                 ObjectsContext *inPDFWriterObjectContext,
                 DocumentContext *inPDFWriterDocumentContext);

private:
  PDFWriter *mPDFWriter;
  PDFAttachmentToObjectIDTypeMap mAttachment;
  void ListenOnCatalogWrite();
  EStatusCodeAndObjectIDType WriteAttachment(PDFAttachment *inAttachment);
  void CleanupAttachment();
};
bool pdf_hummus_make_attachments(string pdf_path,
                                       list<string> attachment_paths,
                                       string out_path) {
  PDFWriter pdfWriter;
  EStatusCode status;
  do {
    status = pdfWriter.ModifyPDF(as_charp(pdf_path), ePDFVersion16,
                                 as_charp(out_path));

    if (status != eSuccess) {
      cout << "start fail\n";
      break;
    }
    PDFAttachmentWriter attachmentWriter(&pdfWriter);

    for (int i = 0; i < (int)N(attachment_paths); i++) {
      string attachment_path = attachment_paths[i];
      InputFileStream tm_file_stream;
      status = tm_file_stream.Open(as_charp(attachment_path));

      if (status != PDFHummus::eSuccess) {
        cout << "failed to open " << attachment_path << "\n";
        continue;
      } else {
        cout << "success to open " << attachment_path << "\n";
      }

      LongFilePositionType file_size = tm_file_stream.GetFileSize();

      Byte *file_content = new Byte[file_size + 16];
      tm_file_stream.Read(file_content, file_size);

      int j, last_slash_index = 0;
      for (j = 0; j < N(attachment_path); j++) {
        if (attachment_path[j] == '/')
          last_slash_index = j;
      }
      string attachment_name =
          attachment_path(last_slash_index + 1, N(attachment_path));

      PDFAttachment *aAttachment =
          new PDFAttachment(file_content, file_size, attachment_name);
      status = attachmentWriter.AttachToAllPage(aAttachment);
      // return status;
      if (status != eSuccess) {
        cout << "fail to attach " << attachment_path << "\n";
        continue;
      } else {
        cout << "success to attach " << attachment_path << "\n";
        continue;
      }
    }
    status = pdfWriter.EndPDF();
  } while (false);

  if (eSuccess == status)
  {
    cout << "Succeeded in creating PDF file\n";
    return true;
  }
  else
  {
    cout << "Failed in creating PDF file\n";
    return false;
  }
}
PDFAttachment::PDFAttachment(void) {
  file_content = NULL;
  lenth = 0;
}

PDFAttachment::PDFAttachment(string attachment_path) {
  InputFileStream tm_file_stream;
  EStatusCode status = tm_file_stream.Open(as_charp(attachment_path));
  if (status != PDFHummus::eSuccess) {
    cout << "failed to open tm\n";
    return;
  }

  lenth = (size_t)tm_file_stream.GetFileSize();

  file_content = new Byte[lenth + 16];
  tm_file_stream.Read(file_content, lenth);

  int i, last_slash_index = 0;
  for (i = 0; i < N(attachment_path); i++) {
    if (attachment_path[i] == '/')
      last_slash_index = i;
  }
  name = attachment_path(last_slash_index + 1, N(attachment_path));
}
PDFAttachment::PDFAttachment(Byte inByte[], size_t inLenth, string inName) {
  file_content = inByte;
  lenth = inLenth;
  name = inName;
}

PDFAttachment::~PDFAttachment(void) {
  if (file_content)
    delete file_content;
}

PDFAttachmentWriter::PDFAttachmentWriter(PDFWriter *inPDFWriter) {
  mPDFWriter = inPDFWriter;
  ListenOnCatalogWrite();
}

void PDFAttachmentWriter::ListenOnCatalogWrite() {
  mPDFWriter->GetDocumentContext().AddDocumentContextExtender(this);
}

PDFAttachmentWriter::~PDFAttachmentWriter(void) {
  if (N(mAttachment) != 0)
    TRACE_LOG("PDFAttachmentWriter::~PDFAttachmentWriter, Exception. Has "
              "attachement that were not associated with pages");
  CleanupAttachment();
}

EStatusCode PDFAttachmentWriter::OnCatalogWrite(
    CatalogInformation *inCatalogInformation,
    DictionaryContext *inCatalogDictionaryContext,
    ObjectsContext *inPDFWriterObjectContext,
    DocumentContext *inPDFWriterDocumentContext) {
  if (N(mAttachment) == 0)
    return eSuccess;

  inCatalogDictionaryContext->WriteKey("Names");

  DictionaryContext *dictionaryContext_0 =
      inPDFWriterObjectContext->StartDictionary();
  dictionaryContext_0->WriteKey("EmbeddedFiles");
  DictionaryContext *dictionaryContext_1 =
      inPDFWriterObjectContext->StartDictionary();
  dictionaryContext_0->WriteKey("Names");
  inPDFWriterObjectContext->StartArray();

  iterator<PDFAttachment *> it = iterate(mAttachment);

  while (it->busy()) {
    PDFAttachment *cur_attachment = it->next();
    inPDFWriterObjectContext->WriteLiteralString(
        as_charp(cur_attachment->name));
    DictionaryContext *dictionaryContext_2 =
        inPDFWriterObjectContext->StartDictionary();
    dictionaryContext_2->WriteKey("EF");
    DictionaryContext *dictionaryContext_3 =
        inPDFWriterObjectContext->StartDictionary();
    dictionaryContext_3->WriteKey("F");
    inPDFWriterObjectContext->WriteIndirectObjectReference(
        mAttachment[cur_attachment]);
    inPDFWriterObjectContext->EndDictionary(dictionaryContext_3);
    dictionaryContext_2->WriteKey("F");
    dictionaryContext_2->WriteLiteralStringValue(
        as_charp(cur_attachment->name));
    dictionaryContext_2->WriteKey("Type");
    dictionaryContext_2->WriteNameValue("F");
    inPDFWriterObjectContext->EndDictionary(dictionaryContext_2);
  }
  inPDFWriterObjectContext->EndArray();
  inPDFWriterObjectContext->EndDictionary(dictionaryContext_1);
  inPDFWriterObjectContext->EndDictionary(dictionaryContext_0);
  CleanupAttachment();
  return eSuccess;
}

void PDFAttachmentWriter::CleanupAttachment() {

  iterator<PDFAttachment *> it = iterate(mAttachment);
  while (it->busy()) {
    delete it->next();
  }
  mAttachment = PDFAttachmentToObjectIDTypeMap();
}

EStatusCode PDFAttachmentWriter::AttachToAllPage(PDFAttachment *inAttachment) {
  return WriteAttachment(inAttachment).first;
}

EStatusCodeAndObjectIDType
PDFAttachmentWriter::WriteAttachment(PDFAttachment *inAttachment) {
  EStatusCodeAndObjectIDType result;
  if (mAttachment->contains(inAttachment)) {
    result.first = eSuccess;
    result.second = mAttachment[inAttachment];
    return result;
  }

  do {
    ObjectsContext &objectsContext = mPDFWriter->GetObjectsContext();

    // Start new InDirect object for annotation dictionary
    result.second = objectsContext.StartNewIndirectObject();

    DictionaryContext *dictionaryContext = objectsContext.StartDictionary();
    dictionaryContext->WriteKey("Type");
    dictionaryContext->WriteNameValue("EmbeddedFile");
    PDFStream *pdfStream = objectsContext.StartPDFStream(dictionaryContext);
    IByteWriter *mwriter = pdfStream->GetWriteStream();
    mwriter->Write(inAttachment->file_content, inAttachment->lenth);
    objectsContext.EndPDFStream(pdfStream);

    mAttachment(inAttachment) = result.second;
    result.first = eSuccess;
  } while (false);
  return result;
}