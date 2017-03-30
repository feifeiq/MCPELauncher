#pragma once
#ifdef __cplusplus
#include <memory>
#include <unordered_map>
#endif
class Entity;
class Player;
#include "mcpe/minecraft.h"
#include "mcpe/blocksource.h"
#include "mcpe/enchant.h"
#include "mcpe/synchedentitydata.h"
#include "mcpe/resourcelocation.h"

#ifdef __cplusplus
#define cppstr std::string
#else
typedef struct {
	char* contents;
} cppstr;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
class EntityUniqueID {
public:
	long long id;
	EntityUniqueID(long long id) : id(id) {
	}

	operator long long() const {
		return this->id;
	}
};
#else
typedef struct {
	long long id;
} EntityUniqueID;
#endif

struct Vec3 {
	float x;
	float y;
	float z;
	Vec3(float x_, float y_, float z_): x(x_), y(y_), z(z_) {
	};
	Vec3(): x(0), y(0), z(0) {
	};
};

struct Vec2 {
	float x;
	float y;
	Vec2(float x_, float y_): x(x_), y(y_) {
	};
};

class AgeableComponent;

enum EntityFlags {
	EntityFlagsImmobile = 16,
};
class PlayerInventoryProxy;
class EntityDamageSource;
// last update: 0.17b2
class Entity {
public:
	void** vtable; //0
	char filler3[64-4]; // 4
	float x; //64
	float y; //68
	float z; //72
	char filler2[100-76]; // 76
	float motionX; //100 found in Entity::rideTick(); should be set to 0 there
	float motionY; //104
	float motionZ; //108
	float pitch; //112 Entity::setRot
	float yaw; //116
	float prevPitch; //120
	float prevYaw; //124

	char filler4[224-128]; //128
	int renderType; //224
	char filler5[420-228]; // 228
	struct Entity* rider; //420 (inaccurate, Entity::getRide() - 4)

	BlockSource* getRegion() const;
	void setRot(Vec2 const&);
	EntityUniqueID const& getUniqueID() const;
	void setNameTag(std::string const&);
	SynchedEntityData* getEntityData();
	EntityUniqueID const& getTargetId();
	void setTarget(Entity*);
	void setStatusFlag(EntityFlags, bool);
	Level* getLevel();
	AgeableComponent* getAgeableComponent() const;
	void hurt(EntityDamageSource const&, int, bool, bool);
	Entity* getRide() const;
	void setNameTagVisible(bool);
};
static_assert(offsetof(Entity, renderType) == 224, "renderType offset wrong");

class Mob: public Entity {
public:
};
enum ContainerID : unsigned char {
	ContainerIDInventory = 0,
};

struct PlayerInventorySlot {
	ContainerID containerID;
	int slot;
};

class PlayerInventoryProxy {
public:
	void add(ItemInstance&, bool);
	void removeResource(ItemInstance const&, bool);
	void clearSlot(int, ContainerID id=ContainerIDInventory);
	ItemInstance* getItem(int, ContainerID id=ContainerIDInventory) const;
	int getLinkedSlot(int) const;
	int getLinkedSlotsCount() const;
	void replaceSlot(int, ItemInstance const&);
	PlayerInventorySlot getSelectedSlot() const;
	void selectSlot(int, ContainerID=ContainerIDInventory);
};
class Player: public Mob {
public:
	int getScore();
	void addExperience(int);
	void addLevels(int);
	PlayerInventoryProxy* getSupplies() const;
};

struct TextureUVCoordinateSet {
	float bounds[4];
	unsigned short size[2]; // 16
	ResourceLocation location;
	TextureUVCoordinateSet(TextureUVCoordinateSet const& other) {
		*this = other; // yeah I know, bad memory management habit. Deal with it
	};
};
//static_assert(offsetof(TextureUVCoordinateSet, textureFile) == 24, "textureFile offset wrong");
static_assert(sizeof(TextureUVCoordinateSet) == 32, "TextureUVCoordinateSet size wrong");

namespace Json {
	class Value;
};
class TextureAtlas;
// Updated 0.14.0b7
// see _Z12registerItemI4ItemIRA11_KciEERT_DpOT0_
// for useAnimation see setUseAnimation
class Item {
public:
	//void** vtable; //0
	char filler0[18-4]; //4
	short itemId; //18
	char filler1[33-20]; // 20
	bool handEquipped; // 33
	char filler[43-34]; //34
	unsigned char useAnimation; // 43
	char filler3[108-44]; // 44
	virtual ~Item();

	static std::unordered_map<std::string, std::pair<std::string, std::unique_ptr<Item>>> mItemLookupMap;
	// this one loads textures
	void initClient(Json::Value&, Json::Value&);
	// this one doesn't
	void initServer(Json::Value&);
	void setStackedByData(bool);
	bool isStackedByData() const;
	void setMaxDamage(int);
	int getMaxDamage();
	static std::shared_ptr<TextureAtlas> mItemTextureAtlas;
};
static_assert(sizeof(Item) == 108, "item size is wrong");

class CompoundTag {
public:
	~CompoundTag();
};

class ItemInstance {
public:
	unsigned char count; //0
	short damage; //2
	unsigned char something; // 4
	char filler[8-5]; // 5
	CompoundTag* tag; // 8
	Item* item; // 12
	void* block; //16

	ItemInstance();
	ItemInstance(int, int, int);
	ItemInstance(ItemInstance const&);
	ItemInstance& operator=(ItemInstance const&);
	bool operator==(ItemInstance const&) const;
	bool operator!=(ItemInstance const&) const;
	ItemEnchants getEnchantsFromUserData() const;
	bool hasCustomHoverName() const;
	std::string getCustomName() const;
	void setCustomName(std::string const&);
	void init(int, int, int);
	int getId() const;
	TextureUVCoordinateSet* getIcon(int, bool) const;
	std::string getName() const;
	int getMaxStackSize() const;
	void remove(int);
}; // see ItemInstance::fromTag for size
// or just use the shared_ptr constructor
static_assert(offsetof(ItemInstance, tag) == 8, "tag offset wrong");
static_assert(offsetof(ItemInstance, item) == 12, "item offset wrong");
static_assert(sizeof(ItemInstance) == 20, "ItemInstance wrong");

enum CreativeItemCategory {
};

enum BlockProperty {
	BlockPropertyOpaque = 32, // from _istransparent
};

class Block {
public:
	void** vtable; //0
	unsigned char id; // 4
	char filler0[8-5]; //5
	std::string nameId; // 8
	std::string mappingId; // 12
	char filler1[20-16]; // 16
	int renderLayer; //20
	int blockProperties; // 24 - getProperties
	char filler2[52-28]; //28
	void* material; //52
	char filler3[80-56]; // 56
	float destroyTime; //80
	float explosionResistance; //84
	char filler4[120-88]; // 88

	float getDestroySpeed() const;
	float getFriction() const;
	void setFriction(float);
	void setSolid(bool);
	void setCategory(CreativeItemCategory);
	std::string const& getDescriptionId() const;
	int getRenderLayer() const;
	static std::unordered_map<std::string, Block*> mBlockLookupMap;
	static Block* mBlocks[0x100];
	static bool mSolid[0x100];
	static float mTranslucency[0x100];
};
static_assert(sizeof(Block) == 120, "Block size is wrong");
static_assert(offsetof(Block, renderLayer) == 20, "renderlayer is wrong");
static_assert(offsetof(Block, explosionResistance) == 84, "explosionResistance is wrong");
#define Tile Block

typedef struct {
	char filler0[172]; //0: from ModelPart::addBox
} Cube;

typedef struct {
	char filler[12]; // 0
} MaterialPtr; // actually mce::MaterialPtr

// from ModelPart::setPos, ModelPart::setTexSize
class ModelPart {
public:
	float offsetX; //0
	float offsetY; //4
	float offsetZ; //8
	float rotateAngleX; // 12
	float rotateAngleY; // 16
	char filler0[37-20]; // 20
	bool showModel; // 37 from HumanoidMobRenderer::prepareArmor
	char filler1[64-38]; //38
	float textureWidth; //64
	float textureHeight; //68
	MaterialPtr* material; //72 from ModelPart::draw
	int textureOffsetX; // 76
	int textureOffsetY; // 80
	char filler2[200-84]; // 84

	void addBox(Vec3 const&, Vec3 const&, float);
}; // 200 bytes
static_assert(sizeof(ModelPart) == 200, "modelpart size wrong");

namespace mce {
	class TexturePtr;
};

// from HumanoidModel::render

class HumanoidModel {
public:
	void** vtable; //0
	char filler[25-4]; // 4
	bool riding; // 25
	char filler1[28-26]; // 26
	MaterialPtr* activeMaterial; // 28
	mce::TexturePtr* activeTexture; // 32 from MobRenderer::renderModel
	MaterialPtr materialNormal; // 36
	MaterialPtr materialAlphaTest; // 48
	MaterialPtr materialAlphaBlend; // 60
	MaterialPtr materialStatic; // 72
	MaterialPtr materialEmissive; // 84
	MaterialPtr materialEmissiveAlpha; // 96
	MaterialPtr materialChangeColor; // 108
	MaterialPtr materialGlint; // 120
	MaterialPtr materialAlphaTestGlint; // 132
	MaterialPtr materialChargedCreeper; // 144
	MaterialPtr materialAlphaTestChangeColor; // 156
	MaterialPtr materialAlphaTestChangeColorGlint; // 168
	MaterialPtr materialMultitexture; // 180
	MaterialPtr materialMultitextureColorMask; // 192
	MaterialPtr materialMultitextureAlphaTest; // 204
	MaterialPtr materialMultitextureAlphaTestColorMask; // 216
	MaterialPtr materialGuardianGhost; // 228
	char filler2[252-240]; // 240
	ModelPart bipedHead;//252
	ModelPart bipedHeadwear;//452
	ModelPart bipedBody;//652
	ModelPart bipedRightArm;//852
	ModelPart bipedLeftArm;//1052
	ModelPart bipedRightLeg;//1252
	ModelPart bipedLeftLeg;//1452
	short unknownshort; // 1652
	char filler3[3868-1654]; // 1654
	HumanoidModel(float, float, int, int);
};

static_assert(sizeof(HumanoidModel) == 3868, "HumanoidModel size");
static_assert(offsetof(HumanoidModel, activeTexture) == 32, "active texture");
static_assert(offsetof(HumanoidModel, materialAlphaTest) == 48, "material alpha test");
static_assert(offsetof(HumanoidModel, bipedHead) == 252, "HumanoidModel bipedHead");

typedef struct {
	Item* item; //0
	Tile* tile; //4
	ItemInstance itemInstance; //8
	char letter; //28
} RecipesType;

typedef struct {
} FurnaceRecipes;

typedef struct {
	char filler[16]; //0
	char* text; //16;
} RakString;

typedef void EntityRenderer;

class MobRenderer {
public:
	void** vtable; //0
	char filler[136-4]; //4
	void* model; // 136 (from MobRenderer::MobRenderer)
	char filler2[624-140]; // 140
	mce::TexturePtr const& getSkinPtr(Entity&) const;
};
static_assert(sizeof(MobRenderer) == 624, "mobrenderer");

typedef void Tag;

typedef struct {
	void** vtable; //0
	char filler[12]; //4
	int64_t value; //16
} LongTag;

#define HIT_RESULT_BLOCK 0
#define HIT_RESULT_ENTITY 1
#define HIT_RESULT_NONE 2

typedef struct {
	int type; //0
	int side; //4
	int x; //8
	int y; //12
	int z; //16
	Vec3 hitVec; //20
	Entity* entity; //32
	unsigned char filler1; //36
} HitResult;

class Biome {
public:
	void** vtable; //0
	char filler[56-4]; //4
	cppstr name; //56 from Biome::setName
	char filler2[148-60]; //60
	int id; //148 from Biome::Biome
};

typedef struct {
	bool invulnerable; // from Desktop Edition's PlayerAbilities and also CreativeMode::initAbilities
	bool flying;
	bool mayFly;
	bool instabuild;
} Abilities;

// from LocalServerListItemElement::serverMainPressed 25c7d4
typedef struct {
	int wtf1; // 0 always negative 1?
	int wtf2; // 4 always negative 1?
	int wtf3; // 8 always 3
	int wtf4; // 12 always 4
	int wtf5; // 16 always 0
	int wtf6; // 20 always 0
	int wtf7; // 24 always 0
	int wtf8; // 28
	int wtf9; // 32
	int wtf10; // 36
} LevelSettings;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} Color;

typedef struct {
	float x1; // 0
	float y1; // 4
	float z1; // 8
	float x2; // 12
	float y2; // 16
	float z2; // 20
	bool shouldBeFalse; // 24
} AABB;

typedef struct {
	unsigned char x;
	unsigned char z;
	unsigned char y;
} ChunkTilePos;

class LevelChunk;

typedef void ModelRenderer;

#ifdef __cplusplus
struct ArmorItem : public Item {
	int armorType; // 108
	int damageReduceAmount; // 112
	int renderIndex; // 116
	void* armorMaterial; // 120
};

#ifdef __arm__
static_assert(sizeof(ArmorItem) == 124, "armor item size");
#endif

struct HumanoidMobRenderer : public MobRenderer {
	int something; // 624
	HumanoidModel* modelArmor; // 628
	HumanoidModel* modelArmorChestplate; // 632
	char hmr_filler1[652-636]; // 636
};
#ifdef __arm__
static_assert(offsetof(HumanoidMobRenderer, modelArmor) == 628, "armour model offset");
static_assert(sizeof(HumanoidMobRenderer) == 652, "humanoid mob renderer size");
#endif
#endif // ifdef __cplusplus

#ifdef __cplusplus
} // extern "C"
#endif

enum GameType {
};

class LevelData {
public:
	void setSpawn(BlockPos const&);
	void setGameType(GameType);
	GameType getGameType() const;
};

class Spawner {
public:
	Entity* spawnItem(BlockSource&, ItemInstance const&, Entity*, Vec3 const&, int);
};

class Level {
public:
	void** vtable;

	Entity* fetchEntity(EntityUniqueID, bool) const;
	void addEntity(BlockSource&, std::unique_ptr<Entity>);
	void addGlobalEntity(BlockSource&, std::unique_ptr<Entity>);
	void explode(BlockSource&, Entity*, Vec3 const&, float, bool, bool, float);
	void setNightMode(bool);
	void setTime(int);
	int getTime() const;
	LevelData* getLevelData();
	void playSound(Vec3 const&, std::string const&, float, float);
	bool isClientSide() const;
	HitResult const& getHitResult();
	int getDifficulty() const;
	Spawner* getSpawner() const;
};

class EntityRenderDispatcher {
public:
	EntityRenderer* getRenderer(Entity&);
	static EntityRenderDispatcher& getInstance();
};

class MinecraftScreenModel {
public:
	void updateTextBoxText(std::string const&);
};

#include "mcpe/blockentity/chestblockentity.h"
#include "mcpe/blockentity/furnaceblockentity.h"
#include "mcpe/blockentity/mobspawnerblockentity.h"
#include "mcpe/blockentity/signblockentity.h"
