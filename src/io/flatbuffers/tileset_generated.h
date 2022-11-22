// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TILESET_TGM_SCHEMA_H_
#define FLATBUFFERS_GENERATED_TILESET_TGM_SCHEMA_H_

#include "flatbuffers/flatbuffers.h"

#include "tile_generated.h"

namespace tgm {
namespace schema {

struct Vector3i;

struct TileWrapper;
struct TileWrapperBuilder;

struct TileSet;
struct TileSetBuilder;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vector3i FLATBUFFERS_FINAL_CLASS {
 private:
  int32_t x_;
  int32_t y_;
  int32_t z_;

 public:
  Vector3i() {
    memset(static_cast<void *>(this), 0, sizeof(Vector3i));
  }
  Vector3i(int32_t _x, int32_t _y, int32_t _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  int32_t x() const {
    return flatbuffers::EndianScalar(x_);
  }
  int32_t y() const {
    return flatbuffers::EndianScalar(y_);
  }
  int32_t z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Vector3i, 12);

struct TileWrapper FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef TileWrapperBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POS = 4,
    VT_T = 6
  };
  const tgm::schema::Vector3i *pos() const {
    return GetStruct<const tgm::schema::Vector3i *>(VT_POS);
  }
  const tgm::schema::Tile *t() const {
    return GetPointer<const tgm::schema::Tile *>(VT_T);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<tgm::schema::Vector3i>(verifier, VT_POS) &&
           VerifyOffset(verifier, VT_T) &&
           verifier.VerifyTable(t()) &&
           verifier.EndTable();
  }
};

struct TileWrapperBuilder {
  typedef TileWrapper Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_pos(const tgm::schema::Vector3i *pos) {
    fbb_.AddStruct(TileWrapper::VT_POS, pos);
  }
  void add_t(flatbuffers::Offset<tgm::schema::Tile> t) {
    fbb_.AddOffset(TileWrapper::VT_T, t);
  }
  explicit TileWrapperBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<TileWrapper> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<TileWrapper>(end);
    return o;
  }
};

inline flatbuffers::Offset<TileWrapper> CreateTileWrapper(
    flatbuffers::FlatBufferBuilder &_fbb,
    const tgm::schema::Vector3i *pos = 0,
    flatbuffers::Offset<tgm::schema::Tile> t = 0) {
  TileWrapperBuilder builder_(_fbb);
  builder_.add_t(t);
  builder_.add_pos(pos);
  return builder_.Finish();
}

struct TileSet FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef TileSetBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_LENGTH = 4,
    VT_WIDTH = 6,
    VT_HEIGHT = 8,
    VT_TILES = 10
  };
  int32_t length() const {
    return GetField<int32_t>(VT_LENGTH, 0);
  }
  int32_t width() const {
    return GetField<int32_t>(VT_WIDTH, 0);
  }
  int32_t height() const {
    return GetField<int32_t>(VT_HEIGHT, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<tgm::schema::TileWrapper>> *tiles() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<tgm::schema::TileWrapper>> *>(VT_TILES);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_LENGTH) &&
           VerifyField<int32_t>(verifier, VT_WIDTH) &&
           VerifyField<int32_t>(verifier, VT_HEIGHT) &&
           VerifyOffset(verifier, VT_TILES) &&
           verifier.VerifyVector(tiles()) &&
           verifier.VerifyVectorOfTables(tiles()) &&
           verifier.EndTable();
  }
};

struct TileSetBuilder {
  typedef TileSet Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_length(int32_t length) {
    fbb_.AddElement<int32_t>(TileSet::VT_LENGTH, length, 0);
  }
  void add_width(int32_t width) {
    fbb_.AddElement<int32_t>(TileSet::VT_WIDTH, width, 0);
  }
  void add_height(int32_t height) {
    fbb_.AddElement<int32_t>(TileSet::VT_HEIGHT, height, 0);
  }
  void add_tiles(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<tgm::schema::TileWrapper>>> tiles) {
    fbb_.AddOffset(TileSet::VT_TILES, tiles);
  }
  explicit TileSetBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<TileSet> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<TileSet>(end);
    return o;
  }
};

inline flatbuffers::Offset<TileSet> CreateTileSet(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t length = 0,
    int32_t width = 0,
    int32_t height = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<tgm::schema::TileWrapper>>> tiles = 0) {
  TileSetBuilder builder_(_fbb);
  builder_.add_tiles(tiles);
  builder_.add_height(height);
  builder_.add_width(width);
  builder_.add_length(length);
  return builder_.Finish();
}

inline flatbuffers::Offset<TileSet> CreateTileSetDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t length = 0,
    int32_t width = 0,
    int32_t height = 0,
    const std::vector<flatbuffers::Offset<tgm::schema::TileWrapper>> *tiles = nullptr) {
  auto tiles__ = tiles ? _fbb.CreateVector<flatbuffers::Offset<tgm::schema::TileWrapper>>(*tiles) : 0;
  return tgm::schema::CreateTileSet(
      _fbb,
      length,
      width,
      height,
      tiles__);
}

}  // namespace schema
}  // namespace tgm

#endif  // FLATBUFFERS_GENERATED_TILESET_TGM_SCHEMA_H_
