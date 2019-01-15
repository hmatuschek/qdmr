#include "ctcssbox.hh"

#define NCTCSS  50
static const int CTCSS_TONES [NCTCSS] = {
     670,  693,  719,  744,  770,  797,  825,  854,  885,  915,
     948,  974, 1000, 1035, 1072, 1109, 1148, 1188, 1230, 1273,
    1318, 1365, 1413, 1462, 1514, 1567, 1598, 1622, 1655, 1679,
    1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928, 1966, 1995,
    2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541,
};

CTCSSBox::CTCSSBox(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("None"), 0.0);
  for (int i=0; i<NCTCSS; i++) {
    double f = double(CTCSS_TONES[i])/10;
    addItem(tr("%1 Hz").arg(f), f);
  }
}
