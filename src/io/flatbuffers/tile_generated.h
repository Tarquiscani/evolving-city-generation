// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TILE_TGM_SCHEMA_H_
#define FLATBUFFERS_GENERATED_TILE_TGM_SCHEMA_H_

#include "flatbuffers/flatbuffers.h"

namespace tgm {
namespace schema {

struct TileBuildingInfo;

struct RoofInfo;

struct Tile;
struct TileBuilder;

enum TileType {
  TileType_none = 0,
  TileType_underground = 1,
  TileType_ground = 2,
  TileType_sky = 3,
  TileType_wooden = 4,
  TileType_grout = 5,
  TileType_MIN = TileType_none,
  TileType_MAX = TileType_grout
};

inline const TileType (&EnumValuesTileType())[6] {
  static const TileType values[] = {
    TileType_none,
    TileType_underground,
    TileType_ground,
    TileType_sky,
    TileType_wooden,
    TileType_grout
  };
  return values;
}

inline const char * const *EnumNamesTileType() {
  static const char * const names[7] = {
    "none",
    "underground",
    "ground",
    "sky",
    "wooden",
    "grout",
    nullptr
  };
  return names;
}

inline const char *EnumNameTileType(TileType e) {
  if (flatbuffers::IsOutRange(e, TileType_none, TileType_grout)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesTileType()[index];
}

enum BorderStyle {
  BorderStyle_none = 0,
  BorderStyle_brickWall = 1,
  BorderStyle_MIN = BorderStyle_none,
  BorderStyle_MAX = BorderStyle_brickWall
};

inline const BorderStyle (&EnumValuesBorderStyle())[2] {
  static const BorderStyle values[] = {
    BorderStyle_none,
    BorderStyle_brickWall
  };
  return values;
}

inline const char * const *EnumNamesBorderStyle() {
  static const char * const names[3] = {
    "none",
    "brickWall",
    nullptr
  };
  return names;
}

inline const char *EnumNameBorderStyle(BorderStyle e) {
  if (flatbuffers::IsOutRange(e, BorderStyle_none, BorderStyle_brickWall)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesBorderStyle()[index];
}

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) TileBuildingInfo FLATBUFFERS_FINAL_CLASS {
 private:
  uint64_t bid_;
  uint64_t aid_;

 public:
  TileBuildingInfo() {
    memset(static_cast<void *>(this), 0, sizeof(TileBuildingInfo));
  }
  TileBuildingInfo(uint64_t _bid, uint64_t _aid)
      : bid_(flatbuffers::EndianScalar(_bid)),
        aid_(flatbuffers::EndianScalar(_aid)) {
  }
  uint64_t bid() const {
    return flatbuffers::EndianScalar(bid_);
  }
  uint64_t aid() const {
    return flatbuffers::EndianScalar(aid_);
  }
};
FLATBUFFERS_STRUCT_END(TileBuildingInfo, 16);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) RoofInfo FLATBUFFERS_FINAL_CLASS {
 private:
  uint64_t bid_;
  uint64_t roof_id_;

 public:
  RoofInfo() {
    memset(static_cast<void *>(this), 0, sizeof(RoofInfo));
  }
  RoofInfo(uint64_t _bid, uint64_t _roof_id)
      : bid_(flatbuffers::EndianScalar(_bid)),
        roof_id_(flatbuffers::EndianScalar(_roof_id)) {
  }
  uint64_t bid() const {
    return flatbuffers::EndianScalar(bid_);
  }
  uint64_t roof_id() const {
    return flatbuffers::EndianScalar(roof_id_);
  }
};
FLATBUFFERS_STRUCT_END(RoofInfo, 16);

struct Tile FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef TileBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_INNER_AREA = 4,
    VT_BLOCK = 6,
    VT_DOOR = 8,
    VT_DOOR_OPEN = 10,
    VT_BORDER_COUNT = 12,
    VT_BUILDING_INFOS = 14,
    VT_TYPE = 16,
    VT_BORDER_STYLE = 18,
    VT_ROOF_INFOS = 20,
    VT_FURNITURE_ID = 22,
    VT_HOSTED_MOBILES = 24
  };
  bool inner_area() const {
    return GetField<uint8_t>(VT_INNER_AREA, 0) != 0;
  }
  uint64_t block() const {
    return GetField<uint64_t>(VT_BLOCK, 0);
  }
  bool door() const {
    return GetField<uint8_t>(VT_DOOR, 0) != 0;
  }
  bool door_open() const {
    return GetField<uint8_t>(VT_DOOR_OPEN, 0) != 0;
  }
  int8_t border_count() const {
    return GetField<int8_t>(VT_BORDER_COUNT, 0);
  }
  const flatbuffers::Vector<const tgm::schema::TileBuildingInfo *> *building_infos() const {
    return GetPointer<const flatbuffers::Vector<const tgm::schema::TileBuildingInfo *> *>(VT_BUILDING_INFOS);
  }
  tgm::schema::TileType type() const {
    return static_cast<tgm::schema::TileType>(GetField<int16_t>(VT_TYPE, 0));
  }
  tgm::schema::BorderStyle border_style() const {
    return static_cast<tgm::schema::BorderStyle>(GetField<int16_t>(VT_BORDER_STYLE, 0));
  }
  const flatbuffers::Vector<const tgm::schema::RoofInfo *> *roof_infos() const {
    return GetPointer<const flatbuffers::Vector<const tgm::schema::RoofInfo *> *>(VT_ROOF_INFOS);
  }
  uint64_t furniture_id() const {
    return GetField<uint64_t>(VT_FURNITURE_ID, 0);
  }
  int16_t hosted_mobiles() const {
    return GetField<int16_t>(VT_HOSTED_MOBILES, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_INNER_AREA) &&
           VerifyField<uint64_t>(verifier, VT_BLOCK) &&
           VerifyField<uint8_t>(verifier, VT_DOOR) &&
           VerifyField<uint8_t>(verifier, VT_DOOR_OPEN) &&
           VerifyField<int8_t>(verifier, VT_BORDER_COUNT) &&
           VerifyOffset(verifier, VT_BUILDING_INFOS) &&
           verifier.VerifyVector(building_infos()) &&
           VerifyField<int16_t>(verifier, VT_TYPE) &&
           VerifyField<int16_t>(verifier, VT_BORDER_STYLE) &&
           VerifyOffset(verifier, VT_ROOF_INFOS) &&
           verifier.VerifyVector(roof_infos()) &&
           VerifyField<uint64_t>(verifier, VT_FURNITURE_ID) &&
           VerifyField<int16_t>(verifier, VT_HOSTED_MOBILES) &&
           verifier.EndTable();
  }
};

struct TileBuilder {
  typedef Tile Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_inner_area(bool inner_area) {
    fbb_.AddElement<uint8_t>(Tile::VT_INNER_AREA, static_cast<uint8_t>(inner_area), 0);
  }
  void add_block(uint64_t block) {
    fbb_.AddElement<uint64_t>(Tile::VT_BLOCK, block, 0);
  }
  void add_door(bool door) {
    fbb_.AddElement<uint8_t>(Tile::VT_DOOR, static_cast<uint8_t>(door), 0);
  }
  void add_door_open(bool door_open) {
    fbb_.AddElement<uint8_t>(Tile::VT_DOOR_OPEN, static_cast<uint8_t>(door_open), 0);
  }
  void add_border_count(int8_t border_count) {
    fbb_.AddElement<int8_t>(Tile::VT_BORDER_COUNT, border_count, 0);
  }
  void add_building_infos(flatbuffers::Offset<flatbuffers::Vector<const tgm::schema::TileBuildingInfo *>> building_infos) {
    fbb_.AddOffset(Tile::VT_BUILDING_INFOS, building_infos);
  }
  void add_type(tgm::schema::TileType type) {
    fbb_.AddElement<int16_t>(Tile::VT_TYPE, static_cast<int16_t>(type), 0);
  }
  void add_border_style(tgm::schema::BorderStyle border_style) {
    fbb_.AddElement<int16_t>(Tile::VT_BORDER_STYLE, static_cast<int16_t>(border_style), 0);
  }
  void add_roof_infos(flatbuffers::Offset<flatbuffers::Vector<const tgm::schema::RoofInfo *>> roof_infos) {
    fbb_.AddOffset(Tile::VT_ROOF_INFOS, roof_infos);
  }
  void add_furniture_id(uint64_t furniture_id) {
    fbb_.AddElement<uint64_t>(Tile::VT_FURNITURE_ID, furniture_id, 0);
  }
  void add_hosted_mobiles(int16_t hosted_mobiles) {
    fbb_.AddElement<int16_t>(Tile::VT_HOSTED_MOBILES, hosted_mobiles, 0);
  }
  explicit TileBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Tile> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Tile>(end);
    return o;
  }
};

inline flatbuffers::Offset<Tile> CreateTile(
    flatbuffers::FlatBufferBuilder &_fbb,
    bool inner_area = false,
    uint64_t block = 0,
    bool door = false,
    bool door_open = false,
    int8_t border_count = 0,
    flatbuffers::Offset<flatbuffers::Vector<const tgm::schema::TileBuildingInfo *>> building_infos = 0,
    tgm::schema::TileType type = tgm::schema::TileType_none,
    tgm::schema::BorderStyle border_style = tgm::schema::BorderStyle_none,
    flatbuffers::Offset<flatbuffers::Vector<const tgm::schema::RoofInfo *>> roof_infos = 0,
    uint64_t furniture_id = 0,
    int16_t hosted_mobiles = 0) {
  TileBuilder builder_(_fbb);
  builder_.add_furniture_id(furniture_id);
  builder_.add_block(block);
  builder_.add_roof_infos(roof_infos);
  builder_.add_building_infos(building_infos);
  builder_.add_hosted_mobiles(hosted_mobiles);
  builder_.add_border_style(border_style);
  builder_.add_type(type);
  builder_.add_border_count(border_count);
  builder_.add_door_open(door_open);
  builder_.add_door(door);
  builder_.add_inner_area(inner_area);
  return builder_.Finish();
}

inline flatbuffers::Offset<Tile> CreateTileDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    bool inner_area = false,
    uint64_t block = 0,
    bool door = false,
    bool door_open = false,
    int8_t border_count = 0,
    const std::vector<tgm::schema::TileBuildingInfo> *building_infos = nullptr,
    tgm::schema::TileType type = tgm::schema::TileType_none,
    tgm::schema::BorderStyle border_style = tgm::schema::BorderStyle_none,
    const std::vector<tgm::schema::RoofInfo> *roof_infos = nullptr,
    uint64_t furniture_id = 0,
    int16_t hosted_mobiles = 0) {
  auto building_infos__ = building_infos ? _fbb.CreateVectorOfStructs<tgm::schema::TileBuildingInfo>(*building_infos) : 0;
  auto roof_infos__ = roof_infos ? _fbb.CreateVectorOfStructs<tgm::schema::RoofInfo>(*roof_infos) : 0;
  return tgm::schema::CreateTile(
      _fbb,
      inner_area,
      block,
      door,
      door_open,
      border_count,
      building_infos__,
      type,
      border_style,
      roof_infos__,
      furniture_id,
      hosted_mobiles);
}

}  // namespace schema
}  // namespace tgm

#endif  // FLATBUFFERS_GENERATED_TILE_TGM_SCHEMA_H_
