
#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"
using namespace Sifteo;


static const unsigned gNumCubes = 3;
static struct MenuItem gItems[] = { { &IconPromoter, &LabelPromoters }, { &IconPromoter, &LabelPromoters }, { &IconPromoter, &LabelPromoters }, { NULL, NULL } };
static struct MenuAssets gAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };
static struct MenuItem hItems[] = { { &IconPromoter, &LabelPromoters }, { &IconPromoter, &LabelPromoters }, { &IconPromoter, &LabelPromoters }, { NULL, NULL } };
static struct MenuAssets hAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };

static Menu menus[gNumCubes];
static VideoBuffer v[gNumCubes];
static struct MenuEvent events[gNumCubes];
static uint8_t cubetouched;

static unsigned gNumCubesConnected = CubeSet::connected().count();

static AssetSlot MainSlot = AssetSlot::allocate()
.bootstrap(BetterflowAssets);

static TiltShakeRecognizer motion[gNumCubes]; //for keeping track of each cube's motion @ev


static Metadata M = Metadata()
.title("helo")
.package("com.sifteo.sdk.menuhello", "1.0.0")
.icon(Icon)
.cubeRange(gNumCubes);

/*EVENTSENSOR CLASS
based on codebase from LSU CCT*/
class EventSensor{
public:
	void install(){
		Events::cubeAccelChange.set(&EventSensor::onAccelChange, this);
	}

private:
	void onAccelChange(unsigned id){
		//CubeID cube(id); //is this creating a cube from the id? 
		//i guess we don't need this? maybe this is for the packet msg?  @ev 

		unsigned changeFlags = motion[id].update(); 
		if (changeFlags){
			if (motion[id].shake){
				LOG("SHAKING\n");
				v[id].attach(id); //shaking gets rid of selected part (i.e. you can scroll menu again) @ev
			}
		}
	}
};


/* BEGIN METHOD
attaches video buffers to all connected cubes*/
static void begin(){	
	for (CubeID cube : CubeSet::connected())
	{
		LOG("cube  %d\n", (int)cube);
		//attaching video buffers 
		auto &vid = v[cube];
		vid.initMode(BG0);
		vid.attach(cube);
		vid.bg0.erase(StripeTile);
		//initializing and attaching motion recognizers 
		motion[cube].attach(cube);

	}
}

/*TOSIDE HELPER METHOD*/
bool isSide(Side aSide){
	return (aSide == RIGHT || aSide == LEFT);
}



/*ADD CUBE METHOD
events to fire when cube is neighboured; 
paramters from doit are carried through*/
void addCube(Menu &m, struct MenuEvent &e, unsigned id){
	LOG("In the addCube method\n");
	if (e.neighbor.masterSide == BOTTOM && e.neighbor.neighborSide == TOP){
		CubeID(id).detachVideoBuffer();
		PCubeID addedCube = e.neighbor.neighbor;
		menus[addedCube].init(v[addedCube], &hAssets, hItems);
	}
	//else if (isSide(e.neighbor.masterSide) && isSide(e.neighbor.neighborSide)){
	//	CubeID(id).detachVideoBuffer();
	//}
}

/* DO IT METHOD
handles event cases, takes in Menu and MenuEvent parameters*/
void doit(Menu &m, struct MenuEvent &e, unsigned id)
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
			addCube(m, e, id);
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
	}
	else {
		ASSERT(e.type == MENU_EXIT);
		m.performDefault();
	}
}



/* MAIN METHOD
contains begin(), initializes the MenuEvent array, 
initializes menus, & contains doit while loop*/
void main(){

	LOG("begin\n");
	begin();
	LOG("after begin\n");

	static EventSensor event;
	event.install();

	struct MenuEvent e;

	for (int i = 0; i < gNumCubesConnected; i++)
	{
		menus[i].init(v[i], &gAssets, gItems);
		menus[i].anchor(0);
	}

	while (1){
		
		for (int i = 0; i < gNumCubesConnected; i++){
			doit(menus[i], events[i], i);
		}
	}

	ASSERT(e.type == MENU_EXIT);
	//m.performDefault();

	LOG("Selected Game: %d\n", e.item);

}