#ifndef __faust2hpp_FaustImpl_H__
#define __faust2hpp_FaustImpl_H__

#include <stdexcept>
#include <unordered_map>

#include "Meta.h"
#include "UI.h"

class FaustImpl : public UI, public Meta
{
public:
  FaustImpl() = default;
  ~FaustImpl() = default;

  FAUSTFLOAT* getParameter(const char* name)
  {
    const auto entry = parameterMap.find(name);
    if (entry == parameterMap.end())
      throw std::invalid_argument(
          std::string("FaustImpl::getParameter: invalid parameter name: ")
          + name);
    return entry->second;
  }

  void setParameter(const char* name, FAUSTFLOAT* value)
  {
    const auto entry = parameterMap.find(name);
    if (entry != parameterMap.end()) entry->second = value;
  }

  // blank implementations for UI
  virtual void openTabBox(const char*){};
  virtual void openHorizontalBox(const char*){};
  virtual void openVerticalBox(const char*){};
  virtual void closeBox(){};
  virtual void addButton(const char*, FAUSTFLOAT*){};
  virtual void addCheckButton(const char*, FAUSTFLOAT*){};
  virtual void addVerticalSlider(
      const char*,
      FAUSTFLOAT*,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT){};
  virtual void addHorizontalSlider(
      const char*,
      FAUSTFLOAT*,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT){};
  virtual void
  // use UI entry to expose user parameters
  addNumEntry(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT)
  {
    if (zone == nullptr) return;
    parameterMap[label] = zone;
  }
  virtual void
  addHorizontalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT){};
  virtual void
  addVerticalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT){};
  virtual void addSoundfile(const char*, const char*, Soundfile**){};

  // blank implememtation for Meta
  virtual void declare(const char*, const char*){};

private:
  std::unordered_map<const char*, FAUSTFLOAT*> parameterMap;
};

#endif
