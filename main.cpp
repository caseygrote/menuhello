
#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"


using namespace Sifteo;


static const unsigned gNumCubes = 3;
static struct MenuItem topItems[] = { { &IconPromoter, &LabelPromoters }, { &IconRBS, &LabelRBS }, { &IconCDS, &LabelCDS }, { &IconTerminator, &LabelTerminators }, { NULL, NULL } };
static struct MenuAssets gAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };
static struct MenuItem promItems[] = { { &IconEcoli, &LabelEcoli }, { &IconYeast, &LabelYeast }, { NULL, NULL } };
static struct MenuItem rbsItems[] = { { &IconConstitutiveProkaryoticRBS, &LabelConstitutiveProkaryoticRBS }, { &IconRiboregulators, &LabelRiboregulators }, { &IconYeast, &LabelYeast }, { NULL, NULL } };
static struct MenuItem cdsItems[] = { { &IconReporters, &LabelReporters }, { &IconSelectionMarkers, &LabelSelectionMarkers }, { &IconTranscriptionalRegulators, &LabelTransciptionalRegulators }, { NULL, NULL } };
static struct MenuItem termItems[] = { { &IconEcoli, &LabelEcoli }, { &IconYeast, &LabelYeast }, { &IconEukaryotic, &LabelEukaryotic }, { NULL, NULL } };
static struct MenuAssets hAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };

static Menu menus[gNumCubes];
static VideoBuffer v[gNumCubes];
static struct MenuEvent events[gNumCubes];
static uint8_t cubetouched;

static unsigned gNumCubesConnected = CubeSet::connected().count();

static AssetSlot MainSlot = AssetSlot::allocate()
.bootstrap(BetterflowAssets);

static TiltShakeRecognizer motion[gNumCubes]; //for keeping track of each cube's motion @ev
static char currentScreen[gNumCubes]; //for keeping track of each cube's current screen @ev
//sort of hacky/non-modular but it works for proof of concept @ev
typedef Array<char[], gNumCubes> currentSearch; //array of character arrays


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
				menus[id].init(v[id], &gAssets, topItems); //brings you back to top level @ev
			}
		}
	}
};

/*TREE CLASS
data structure for holding items*/
class Tree{
public: 
	MenuItem* items;
	Tree();
	Tree(MenuItem* itemList){
		items = itemList;
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

void nextLevel(PCubeID addedCube, char current){
	if (current == 'p'){
		menus[addedCube].init(v[addedCube], &hAssets, promItems);
	}
	else if (current == 'r'){
		menus[addedCube].init(v[addedCube], &hAssets, rbsItems);
	}
	else if (current == 'c'){
		menus[addedCube].init(v[addedCube], &hAssets, cdsItems);
	}
	else {
		menus[addedCube].init(v[addedCube], &hAssets, termItems);
	}
}



/*ADD CUBE METHOD
events to fire when cube is neighboured; 
paramters from doit are carried through*/
void addCube(Menu &m, struct MenuEvent &e, unsigned id, char current){
	LOG("In the addCube method\n");
	if (e.neighbor.masterSide == BOTTOM && e.neighbor.neighborSide == TOP){
		CubeID(id).detachVideoBuffer();
		PCubeID addedCube = e.neighbor.neighbor;
		nextLevel(addedCube, current);
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
			addCube(m, e, id, currentScreen[id]);
			break;

		case MENU_NEIGHBOR_REMOVE:
			LOG("lost cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			if (e.neighbor.masterSide == BOTTOM && e.neighbor.neighborSide == TOP){
				v[id].attach(id);
			}
			break;

		case MENU_ITEM_ARRIVE:
			LOG("arriving at menu item %d\n", e.item);
			//if statement for current screen array @ev
			if (e.item == 0){
				currentScreen[id] = 'p';
				LOG("p\n");
			}
			else if (e.item == 1){
				currentScreen[id] = 'r';
				LOG("r\n");
			}
			else if (e.item == 2){
				currentScreen[id] = 'c';
				LOG("c\n");
			}
			else {
				currentScreen[id] = 't';
				LOG("t\n");
			}
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
	//TESTING CODE FOR TREE: 
	Tree myFuck = Tree(topItems); 

	LOG("begin\n");
	begin();
	LOG("after begin\n");

	static EventSensor event;
	event.install();

	struct MenuEvent e;

	for (int i = 0; i < gNumCubesConnected; i++)
	{
		menus[i].init(v[i], &gAssets, topItems);
		menus[i].anchor(0);
		currentScreen[i] = 'p'; //proof of concept using char array; why is the string array so hard to invoke im so confused???????? @ev
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