#include "gd77_codeplug.hh"
#include "config.hh"

#define OFFSET_TIMESTMP     0x00088
#define OFFSET_SETTINGS     0x000e0
#define OFFSET_MSGTAB       0x00128
#define OFFSET_SCANTAB      0x01790
#define OFFSET_BANK_0       0x03780 // Channels 1-128
#define OFFSET_INTRO        0x07540
#define OFFSET_ZONETAB      0x08010
#define OFFSET_BANK_1       0x0b1b0 // Channels 129-1024
#define OFFSET_CONTACTS     0x17620
#define OFFSET_GROUPTAB     0x1d620

#define GET_TIMESTAMP()     (&radio_mem[OFFSET_TIMESTMP])
#define GET_SETTINGS()      ((general_settings_t*) &radio_mem[OFFSET_SETTINGS])
#define GET_INTRO()         ((intro_text_t*) &radio_mem[OFFSET_INTRO])
#define GET_ZONETAB()       ((zonetab_t*) &radio_mem[OFFSET_ZONETAB])
#define GET_SCANTAB(i)      ((scantab_t*) &radio_mem[OFFSET_SCANTAB])
#define GET_GROUPTAB()      ((grouptab_t*) &radio_mem[OFFSET_GROUPTAB])
#define GET_MSGTAB()        ((msgtab_t*) &radio_mem[OFFSET_MSGTAB])
#define GET_CONTACT(i)      ((contact_t*) &radio_mem[OFFSET_CONTACTS + (i)*24])

#define VALID_TEXT(txt)     (*(txt) != 0 && *(txt) != 0xff)
#define VALID_CONTACT(ct)   ((ct) != 0 && VALID_TEXT((ct)->name))


uint32_t
GD77Codeplug::contact_t::getId() const {
  return ((id[0] >> 4) * 10000000 +\
      (id[0] & 15) * 1000000 +\
      (id[1] >> 4) * 100000 +\
      (id[1] & 15) * 10000 +\
      (id[2] >> 4) * 1000 +\
      (id[2] & 15) * 100 +\
      (id[3] >> 4) * 10 +\
      (id[3] & 15));
}


GD77Codeplug::GD77Codeplug(QObject *parent)
  : CodePlug(parent)
{
  addImage("Radioddity GD77 Codeplug");
  image(0).addElement(0x00080, 0x07b80);
  image(0).addElement(0x08000, 0x16300);
}

bool
GD77Codeplug::encode(Config *config) {
  return false;
}

bool
GD77Codeplug::decode(Config *config) {
  // Clear config object
  config->reset();

  return false;
}
