
#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"
#include <sifteo/usb.h>
#include <sifteo/math.h>


using namespace Sifteo;


static const unsigned gNumCubes = 3;
static struct MenuItem geneItems[] = { { &BG, NULL}, { &YG, NULL}, { &RG, NULL}, {NULL, NULL}};
static struct MenuItem ecoliItems[] = { { &nul, NULL}, {NULL, NULL}};
static struct MenuItem plasmidInit[] = { { &Plasmid, &LabelEmpty }, { NULL, NULL } };
static struct MenuAssets cubeAssets = { &WhiteTile, NULL, NULL, {NULL} };
static struct MenuItem BCTi[] = {{&BCT, NULL}, {NULL, NULL}};
static struct MenuItem RCTi[] = {{&RCT, NULL}, {NULL, NULL}};
static struct MenuItem YCTi[] = {{&YCT, NULL}, {NULL, NULL}};
static struct MenuItem BCLi[] = {{&BCL, NULL}, {NULL, NULL}};
static struct MenuItem RCLi[] = {{&RCL, NULL}, {NULL, NULL}};
static struct MenuItem YCLi[] = {{&YCL, NULL}, {NULL, NULL}};
static struct MenuItem BCRi[] = {{&BCR, NULL}, {NULL, NULL}};
static struct MenuItem RCRi[] = {{&RCR, NULL}, {NULL, NULL}};
static struct MenuItem YCRi[] = {{&YCR, NULL}, {NULL, NULL}};
static struct MenuItem BCBi[] = {{&BCB, NULL}, {NULL, NULL}};
static struct MenuItem RCBi[] = {{&RCB, NULL}, {NULL, NULL}};
static struct MenuItem YCBi[] = {{&YCB, NULL}, {NULL, NULL}};
static struct MenuItem PCT[] = {{&pcTop, NULL}, {NULL, NULL}};
static struct MenuItem PCB[] = {{&pcBot, NULL}, {NULL, NULL}};
static struct MenuItem PCL[] = {{&pcL, NULL}, {NULL, NULL}};
static struct MenuItem PCR[] = {{&pcR, NULL}, {NULL, NULL}};
static struct MenuItem blueFill[] = {{&BFPlasmid, NULL}, {NULL, NULL}};
static struct MenuItem yellowFill[] = {{&YFPlasmid, NULL}, {NULL, NULL}};
static struct MenuItem redFill[] = {{&RFPlasmid, NULL}, {NULL, NULL}};



static Menu menus[gNumCubes];
static struct MenuEvent events[gNumCubes];
static uint8_t cubetouched;
static bool flipped[gNumCubes] = { false };
static VideoBuffer v[gNumCubes];
static unsigned geneItem = 0;
unsigned neighbored[3] = {0};
unsigned frame = 0;
const Float2 center = { 64 - 12.5f, 64 - 18.5f };
 float fpsTimespan;
 unsigned plasFill = 0;

static unsigned gNumCubesConnected = CubeSet::connected().count();

static AssetLoader loader; // global asset loader (each cube will have symmetric assets)
static AssetConfiguration<2> config; // global asset configuration (will just hold the bootstrap group)

static AssetSlot MainSlot = AssetSlot::allocate()
.bootstrap(BetterflowAssets);

static AssetSlot SecondSlot = AssetSlot::allocate()
	.bootstrap(TestAssets);

static TiltShakeRecognizer motion[gNumCubes]; //for keeping track of each cube's motion @ev
static unsigned currentStep[gNumCubes]; //for keeping track of each cube's current step in the process (e.g. do we need to shake one more time?) @ev



static Metadata M = Metadata()
.title("synflo")
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

		unsigned changeFlags = motion[id].update(); 
		if (changeFlags){
			if (motion[id].shake){
				LOG("SHAKING\n");
				//if (id == 1 && plasFill == 0 && neighbored[1] == 1){
				//	LOG("happening\n");
				//	TimeStep ts;
				//	while (frame < 500) {
			
				//	//update(id, ts.delta());

				//	System::paint();
				//	ts.next();
				//	}
				//frame = 0;
				//plasFill == 1;
				//}
			}
			else if (motion[id].Tilt_ZChange){
				if (motion[id].tilt.z == -1 && motion[id].tilt.x == 0 && motion[id].tilt.y == 0 && !flipped[id]){
					flipped[id] = true;
					LOG("flipped\n");
				}
				else if (motion[id].tilt.z == 1 && motion[id].tilt.x == 0 && motion[id].tilt.y == 0 && flipped[id]){
					flipped[id] = false;
					LOG("flipped back\n");
				}
			}
		}
	}

	/*void update(CubeID id, TimeDelta timeStep){		
		frame++;
        fpsTimespan += timeStep;
		v[1].sprites[0].setImage(Loop, (frame) % Loop.numFrames());	//animated pikachu
		//v[1].sprites[0].move(center);
	} */
};

	void update(CubeID id, TimeDelta timeStep){		
		frame++;
        fpsTimespan += timeStep;
		if (frame % 2 == 0){
			if (id == 2) {
				v[id].sprites[0].setImage(Loop, (frame) % Loop.numFrames());
			}
		}
		//v[1].sprites[0].move(center);
	}


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

/*GENE TO PLASMID
helper method fires when cube 0 is neighbored with cube 1 */
void geneToPlasmid(MenuEvent e){
	LOG("yes hello");
	if (geneItem == 0){
		if (e.neighbor.masterSide == LEFT){
			menus[0].init(v[0], &cubeAssets, BCLi);
		}else if (e.neighbor.masterSide == RIGHT){
		menus[0].init(v[0], &cubeAssets, BCRi);
		} else if (e.neighbor.masterSide == TOP) {
		menus[0].init(v[0], &cubeAssets, BCTi);
		} else {
		menus[0].init(v[0], &cubeAssets, BCBi);
		}
	} else if (geneItem == 1) {
			if (e.neighbor.masterSide == LEFT){
		menus[0].init(v[0], &cubeAssets, YCLi);
	} else if (e.neighbor.masterSide == RIGHT){
		menus[0].init(v[0], &cubeAssets, YCRi);
	} else if (e.neighbor.masterSide == TOP) {
		menus[0].init(v[0], &cubeAssets, YCTi);
	} else {
		menus[0].init(v[0], &cubeAssets, YCBi);
		}
	} else {
			if (e.neighbor.masterSide == LEFT){
		menus[0].init(v[0], &cubeAssets, RCLi);
	} else if (e.neighbor.masterSide == RIGHT){
		menus[0].init(v[0], &cubeAssets, RCRi);
	} else if (e.neighbor.masterSide == TOP) {
		menus[0].init(v[0], &cubeAssets, RCTi);
	} else {
		menus[0].init(v[0], &cubeAssets, RCBi);
	}
	}

}

/*PLASMID ORIENT
helper method points plasmid connect in the right direction */
void plasmidOrient(MenuEvent e){
	if (e.neighbor.neighborSide == LEFT){
	menus[1].init(v[1], &cubeAssets, PCL);
	} else if (e.neighbor.neighborSide == RIGHT) {
		menus[1].init(v[1], &cubeAssets, PCR);
	} else if (e.neighbor.neighborSide == TOP){
		menus[1].init(v[1], &cubeAssets, PCT);
	} else {
		menus[1].init(v[1], &cubeAssets, PCB);
	}
}

void fillPlasmid(){
	if (geneItem == 0){
		menus[1].init(v[1], &cubeAssets, blueFill);
	} else if (geneItem == 1){
		menus[1].init(v[1], &cubeAssets, yellowFill);
	} else {
		menus[1].init(v[1], &cubeAssets, redFill);
	}
}


/* DO IT METHOD
handles event cases, takes in Menu and MenuEvent parameters*/
void __declspec(noinline) doit(Menu &m, struct MenuEvent &e, unsigned id)
{
	if (m.pollEvent(&e)){

		switch (e.type){

			{ case MENU_ITEM_PRESS:
				break;
			}

			{
			case MENU_EXIT:
				ASSERT(false);
				break;
		}

			{case MENU_NEIGHBOR_ADD:
				LOG("found cube %d on side %d of menu (neighbor's %d side)\n",
					e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
					
				
					if (id == 0 && e.neighbor.neighbor == 1){ //for putting gene in plasmid @ev
						if (neighbored[id] == 0 && plasFill == 0){
							geneToPlasmid(e);
							plasmidOrient(e);
						}
					} else if (id == 1 && e.neighbor.neighbor == 2){ //for inserting plasmid into ecoli @ev
						//plasmidToEcoli();
					}
					neighbored[id] = 1;
				break;

			}

			{case MENU_NEIGHBOR_REMOVE:
				LOG("lost cube %d on side %d of menu (neighbor's %d side)\n",
					e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
				neighbored[id] = 0;
				if (id == 0) {
					menus[0].init(v[0], &cubeAssets, geneItems);
					menus[0].anchor(geneItem);
				} else if (id == 1) {
					v[1].sprites[0].hide();
					if (plasFill == 1){
						fillPlasmid();
					} else {
						menus[1].init(v[1], &cubeAssets, plasmidInit);
					}
				}
				break;
			}

			{case MENU_ITEM_ARRIVE:
				if (id == 0 && neighbored[0] == 0){ //stores current gene item @ev
					geneItem = e.item;
					LOG("gene item is %d\n", geneItem);
				}
				break;
			}

			{case MENU_ITEM_DEPART:
				break;
			}

			{case MENU_PREPAINT:
				// do your implementation-specific drawing here
				// NOTE: this event should never have its default handler skipped.
				break;
			}

			{case MENU_UNEVENTFUL:
				ASSERT(false);
				break;
			}

		}
		m.performDefault();
	}
	else {
		//ASSERT(e.type == MENU_EXIT);
		m.performDefault();
	}
}


/* MAIN METHOD
contains begin(), initializes the MenuEvent array, 
initializes menus, & contains doit while loop*/
void main(){

	begin();

	static EventSensor event;
	event.install();

	struct MenuEvent e;

	menus[0].init(v[0], &cubeAssets, geneItems); //the genes 

	//v[0].bg0.image(vec(0,0), IconPromoter);



	menus[1].init(v[1], &cubeAssets, plasmidInit); //the plasmid 

	menus[2].init(v[2], &cubeAssets, ecoliItems); 

	TimeStep ts;

	while (1){
		
		for (int i = 0; i < gNumCubesConnected; i++){
			doit(menus[i], events[i], i);
		}
		
		update(2, ts.delta());
		System::paint();
		ts.next();
	}

}
