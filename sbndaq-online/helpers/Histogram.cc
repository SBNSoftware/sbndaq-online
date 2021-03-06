#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TImage.h"

#include "Histogram.h"
#include "Binary.h"

void sbndaq::SendHistogram(const std::string &key, TH1 *hist, size_t padding_x, size_t padding_y, size_t size_x, size_t size_y) {
  TCanvas *c = new TCanvas;
  hist->Draw();
  TImage *img = TImage::Create();
  img->FromPad(c, padding_x, padding_y, size_x, size_y);
  char *image_data = NULL;
  int image_len = -1;

  img->GetImageBuffer(&image_data, &image_len); 
  SendBinary(key, image_data, image_len);
  delete c; 
  delete img;
}

void sbndaq::SendHistogram(const std::string &key, TH2 *hist, size_t padding_x, size_t padding_y, size_t size_x, size_t size_y) {
  TCanvas *c = new TCanvas;
  hist->Draw();
  TImage *img = TImage::Create();
  img->FromPad(c, padding_x, padding_y, size_x, size_y);
  char *image_data = NULL;
  int image_len = -1;

  img->GetImageBuffer(&image_data, &image_len); 
  SendBinary(key, image_data, image_len);
  delete c; 
  delete img;
}

