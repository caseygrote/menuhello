
#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"
using namespace Sifteo;


static const unsigned gNumCubes = 3;
static struct MenuItem gItems[] = { { &IconSBOL, &LabelChroma }, { &IconPromoter, &LabelSandwich }, { &IconPromoter, NULL }, { NULL, NULL } };
static struct MenuAssets gAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };

static Menu menus[gNumCubes];
static VideoBuffer v[gNumCubes];
static struct MenuEvent events[gNumCubes];
static uint8_t cubetouched;

static unsigned gNumCubesConnected = CubeSet::connected().count();

static AssetSlot MainSlot = AssetSlot::allocate()
.bootstrap(BetterflowAssets);


static Metadata M = Metadata()
.title("helo")
.package("com.sifteo.sdk.menuhello", "1.0.0")
.icon(Icon)
.cubeRange(gNumCubes);

static void begin(){
	//attach video buffers ?
	

	for (CubeID cube : CubeSet::connected())
	{
		//LOG("cube  %d\n", (int)cube);
		auto &vid = v[cube];
		vid.initMode(BG0);
		vid.attach(cube);
		vid.bg0.erase(StripeTile);

	}
}

void doit(Menu &m, struct MenuEvent &e)
{
	if (m.pollEvent(&e)){

		switch (e.type){

		case MENU_ITEM_PRESS:
			m.anchor(e.item);
			break;

		case MENU_EXIT:
			ASSERT(false);
			break;

		case MENU_NEIGHBOR_ADD:
			LOG("found cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			break;

		case MENU_NEIGHBOR_REMOVE:
			LOG("lost cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			break;

		case MENU_ITEM_ARRIVE:
			LOG("arriving at menu item %d\n", e.item);
			break;

		case MENU_ITEM_DEPART:
			break;

		case MENU_PREPAINT:
			// do your implementation-specific drawing here
			// NOTE: this event should never have its default handler skipped.
			break;

		case MENU_UNEVENTFUL:
			ASSERT(false);
			break;

		}
		m.performDefault();
		//return NOT_SELECTED;
	}else
	{
		ASSERT(e.type == MENU_EXIT);
		m.performDefault();

		LOG("Selected Game: %d\n", e.item);
		//return e.item;
	}
}

void main(){
	LOG("begin");
	begin();
	LOG("after begin");


	struct MenuEvent e;


	for (int i = 0; i < gNumCubesConnected; i++)
	{
		menus[i].init(v[i], &gAssets, gItems);
		menus[i].anchor(i);
		
	}

	while (1){
		for (int i = 0; i < gNumCubesConnected; i++){
			doit(menus[i], events[i]);
		}
	}
	
	ASSERT(e.type == MENU_EXIT);
	LOG("ABOUT TO DO SOME CRAZY SHITE");
	//m.performDefault();

	LOG("Selected Game: %d\n", e.item);

}