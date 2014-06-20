
#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"
#include <node.h>


using namespace Sifteo;


static const unsigned gNumCubes = 3;
static struct MenuItem noItems[] = { { &NoMore, &LabelEmpty }, { NULL, NULL } };
static struct MenuItem topItems[] = { { &IconPromoter, &LabelPromoters }, { &IconRBS, &LabelRBS }, { &IconCDS, &LabelCDS }, { &IconTerminator, &LabelTerminators }, { NULL, NULL } };
static struct MenuItem promItems[] = { { &IconEcoli, &LabelEcoli }, { &IconYeast, &LabelYeast }, { NULL, NULL } };
static struct MenuItem rbsItems[] = { { &IconConstitutiveProkaryoticRBS, &LabelConstitutiveProkaryoticRBS }, { &IconRiboregulators, &LabelRiboregulators }, { &IconYeast, &LabelYeast }, { NULL, NULL } };
static struct MenuItem cdsItems[] = { { &IconReporters, &LabelReporters }, { &IconSelectionMarkers, &LabelSelectionMarkers }, { &IconTranscriptionalRegulators, &LabelTransciptionalRegulators }, { NULL, NULL } };
static struct MenuItem termItems[] = { { &IconEcoli, &LabelEcoli }, { &IconYeast, &LabelYeast }, { &IconEukaryotic, &LabelEukaryotic }, { NULL, NULL } };
static struct MenuItem promEcoliYeast[] = { { &IconPositivePromoter, &LabelPositive }, { &IconConsitutitive, &LabelConsitutitive }, { &IconNegativePromoter, &LabelNegative }, { &IconMultiregulated, &LabelMultiregulated }, { NULL, NULL } };
static struct MenuItem rbsConst[] = { { &IconBBa_J61100, &LabelBBa_J61100 }, { &IconBBa_J61101, &LabelBBa_J61101 }, { NULL, NULL } };
static struct MenuItem rbsRibo[] = { { &IconBBa_J01010, &LabelBBa_J01010 }, { &IconBBa_J01080, &LabelBBa_J01080 }, { NULL, NULL } };
static struct MenuItem rbsYeast[] = { { &IconBBa_J63003, &LabelBBa_J63003 }, { &IconBBa_K165002, &LabelBBa_K165002 }, { NULL, NULL } };
static struct MenuItem cdsReport[] = { { &IconChromoproteins, &LabelChromoproteins }, { &IconFluorescentProteins, &LabelFluorescentProteins }, { NULL, NULL } };
static struct MenuItem cdsSelect[] = { { &IconBBa_K389005, &LabelBBa_K389005 }, { &IconBBa_J31005, &LabelBBa_J31005 }, { NULL, NULL } };
static struct MenuItem cdsTrans[] = { { &IconActivators, &LabelEmpty }, { &IconRepressor, &LabelEmpty }, { &IconMultiple, &LabelEmpty }, { NULL, NULL } };
static struct MenuItem termEcoli[] = { { &IconForward, &LabelForward }, { &IconReverse, &LabelReverse }, { NULL, NULL } };
static struct MenuItem termYeast[] = { { &IconBBa_J63002, &LabelBBa_J63002 }, { &IconBBa_K110012, &LabelBBa_K110012 }, { NULL, NULL } };
static struct MenuItem termEuk[] = { { &IconBBa_J52016, &LabelBBa_J52016 }, { &IconBBa_J63002, &LabelBBa_J63002 }, { NULL, NULL } };
static struct MenuItem promEcPos[] = { { &IconBBa_I0500, &LabelBBa_I0500 }, { NULL, NULL } };
static struct MenuItem promEcConst[] = { { &IconBBa_I14018, &LabelBBa_I14018 }, { &IconBBa_I14033, &LabelBBa_I14033 }, { NULL, NULL } };
static struct MenuItem promEcNeg[] = { { &IconBBa_I1051, &LabelBBa_I1051 }, { NULL, NULL } };
static struct MenuItem promYePos[] = { { &IconBBa_J63006, &LabelBBa_J63006 }, { NULL, NULL } };
static struct MenuItem promYeConst[] = { { &IconBBa_I766555, &LabelBBa_I766555 }, { NULL, NULL } };
static struct MenuItem promYeNeg[] = { { &IconBBa_K950000, &LabelBBa_K950000 }, { &IconBBa_K950002, &LabelBBa_K950002 }, { NULL, NULL } };
static struct MenuItem promYeMulti[] = { { &IconBBa_I766200, &LabelBBa_I766200 }, { NULL, NULL } };
static struct MenuItem cdsRepChromo[] = { { &IconBBa_K592009, &LabelBBa_K592009 }, { &IconBBa_K592011, &LabelBBa_K592011 }, { &IconBBa_K592012, &LabelBBa_K592012 }, { NULL, NULL } };
static struct MenuItem cdsRepFluor[] = { { &IconBBa_E0030, &LabelBBa_E0030 }, { &IconBBa_E0020, &LabelBBa_E0020 }, { NULL, NULL } };
static struct MenuItem cdsTransAct[] = { { &IconBBa_C0079, &LabelBBa_C0079 }, { NULL, NULL } };
static struct MenuItem cdsTransRep[] = { { &IconBBa_C0012, &LabelBBa_C0012 }, { NULL, NULL } };
static struct MenuItem cdsTransMult[] = { { &IconBBa_C0062, &LabelBBa_C0062 }, { NULL, NULL } };
static struct MenuItem termEcFor[] = { { &IconBBa_B0010, &LabelBBa_B0010 }, { NULL, NULL } };
static struct MenuItem termEcRev[] = { { &IconBBa_B0020, &LabelBBa_B0020 }, { NULL, NULL } };
static struct MenuItem termEcBi[] = { { &IconBBa_B0011, &LabelBBa_B0011 }, { &IconBBa_B0014, &LabelBBa_B0014 }, { NULL, NULL } };
static struct MenuAssets cubeAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };

static Menu menus[gNumCubes];
static VideoBuffer v[gNumCubes];
static struct MenuEvent events[gNumCubes];
static uint8_t cubetouched;

//NODES: 
static const unsigned numNodes = 32;
static Node nodeItems[numNodes]; //all node items 
static Node currentNode[gNumCubes + 1]; //node items assoc. with cubes 

static unsigned gNumCubesConnected = CubeSet::connected().count();

static AssetSlot MainSlot = AssetSlot::allocate()
.bootstrap(BetterflowAssets);

static TiltShakeRecognizer motion[gNumCubes]; //for keeping track of each cube's motion @ev
static unsigned currentScreen[gNumCubes]; //for keeping track of each cube's current screen @ev
//sort of hacky/non-modular but it works for proof of concept @ev
typedef Array<char[], gNumCubes> currentSearch; //array of character arrays

static const unsigned numTrees = 32;



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
				menus[id].init(v[id], &cubeAssets, topItems); //brings you back to top level @ev
				//***************************************NEED TO FIGURE OUT HOW TO ASSIGN TREE AT THIS LEVEL @EV**************************************************************************
			}
		}
	}
};


/*TREE CLASS
data structure for holding items*/
class Tree{
public: 
	Tree(){};
	Tree(MenuItem* itemList, MenuAssets* assetList, unsigned levelInt){
		items = itemList;
		level = levelInt;
		assets = assetList;
	}

	void setChildren(Tree* kiddies){
		children = kiddies;
	}

	Tree* getChildren(){
		return children;
	}

	void setAssets(MenuAssets* assetList){
		assets = assetList;
	}

	MenuAssets* getAssets(){
		return assets;
	}

	void setMenu(MenuItem* itemList){
		items = itemList;
	}

	MenuItem* getMenu(){
		return items;
	}

private:
	MenuItem* items;
	MenuAssets* assets;
	unsigned level;
	Tree* children;
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
		//currentNode[cube] = nodeItems[0]; //attaching nodes @ev
	}
}

/*LEVEL HELPER METHOD
uses tree structure instead of character array*/
void level(unsigned id, PCubeID addedCube){
	LOG("In level method\n");
	unsigned screen = currentScreen[id];
	Node tr = currentNode[id];
	if (tr.getChildren() == NULL) {
		LOG("NO CHILDREN\n");
		Node noMore = currentNode[gNumCubes];
		menus[addedCube].init(v[addedCube], noMore.getAssets(), noMore.getMenu());
		currentNode[addedCube] = noMore;
	}
	else {
		Node newtr = tr.getChildren()[screen];
		if (currentNode[addedCube].getMenu() != newtr.getMenu()){
			menus[addedCube].init(v[addedCube], newtr.getAssets(), newtr.getMenu());
			currentNode[addedCube] = newtr;
		}
	}
}

/* PLUS CUBE HELPER METHOD
uses tree structure instead of character array; 
fires when cube is neighboured*/
void plusCube(unsigned id, struct MenuEvent e){
	LOG("In the plusCube method\n");
	if (e.neighbor.masterSide == BOTTOM && e.neighbor.neighborSide == TOP){
		CubeID(id).detachVideoBuffer();
		PCubeID addedCube = e.neighbor.neighbor;
		level(id, addedCube);
	}
}


/* DO IT METHOD
handles event cases, takes in Menu and MenuEvent parameters*/
void doit(Menu &m, struct MenuEvent &e, unsigned id)
{
	if (m.pollEvent(&e)){

		switch (e.type){

		case MENU_ITEM_PRESS:
			m.anchor(e.item);
			/*if (currTree[id].getChildren() == NULL){
				CubeID(id).detachVideoBuffer();
			} */
			LOG("MENU ITEM BEING PRESSED NAO\n");
			break;

		case MENU_EXIT:
			ASSERT(false);
			break;

		case MENU_NEIGHBOR_ADD:
			LOG("found cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			plusCube(id, e);
			break;

		case MENU_NEIGHBOR_REMOVE:
			LOG("lost cube %d on side %d of menu (neighbor's %d side)\n",
				e.neighbor.neighbor, e.neighbor.masterSide, e.neighbor.neighborSide);
			if (e.neighbor.masterSide == BOTTOM && e.neighbor.neighborSide == TOP){
				v[id].attach(id);
			}
			break;

		case MENU_ITEM_ARRIVE:
			//LOG("arriving at menu item %d\n", e.item);
			currentScreen[id] = e.item;
			break;

		case MENU_ITEM_DEPART:
			break;

		case MENU_PREPAINT:
			// do your implementation-specific drawing here
			// NOTE: this event should never have its default handler skipped.
			break;

		case MENU_UNEVENTFUL:
			ASSERT(false);
			LOG("MENU ITEM UNEVENTFUL??\n");
			break;

		}
		m.performDefault();
	}
	else {
		ASSERT(e.type == MENU_EXIT);
		m.performDefault();
	}
}


/* ASSIGN TREES METHOD
for assigning menus to tree objects*/
void assign_Nodes(Node* nodeArray){
	//LEVEL 0
	nodeArray[0] = Node(topItems, &cubeAssets, 0);
	nodeArray[31] = Node(noItems, &cubeAssets, 0);

	//LEVEL 1
	nodeArray[1] = Node(promItems, &cubeAssets, 1);
	nodeArray[2] = Node(rbsItems, &cubeAssets, 1);
	nodeArray[3] = Node(cdsItems, &cubeAssets, 1);
	nodeArray[4] = Node(termItems, &cubeAssets, 1);

	//LEVEL 2
	nodeArray[5] = Node(promEcoliYeast, &cubeAssets, 2);
	nodeArray[6] = Node(promEcoliYeast, &cubeAssets, 2);

	nodeArray[7] = Node(rbsConst, &cubeAssets, 2);
	nodeArray[8] = Node(rbsRibo, &cubeAssets, 2);
	nodeArray[9] = Node(rbsYeast, &cubeAssets, 2);

	nodeArray[10] = Node(cdsReport, &cubeAssets, 2);
	nodeArray[11] = Node(cdsSelect, &cubeAssets, 2);
	nodeArray[12] = Node(cdsTrans, &cubeAssets, 2);

	nodeArray[13] = Node(termEcoli, &cubeAssets, 2);
	nodeArray[14] = Node(termYeast, &cubeAssets, 2);
	nodeArray[15] = Node(termEuk, &cubeAssets, 2);

	//LEVEL 3
	nodeArray[16] = Node(promEcPos, &cubeAssets, 3);
	nodeArray[17] = Node(promEcConst, &cubeAssets, 3);
	nodeArray[18] = Node(promEcNeg, &cubeAssets, 3);

	nodeArray[19] = Node(promYePos, &cubeAssets, 3);
	nodeArray[20] = Node(promYeConst, &cubeAssets, 3);
	nodeArray[21] = Node(promYeNeg, &cubeAssets, 3);
	nodeArray[22] = Node(promYeMulti, &cubeAssets, 3);

	nodeArray[23] = Node(cdsRepChromo, &cubeAssets, 3);
	nodeArray[24] = Node(cdsRepFluor, &cubeAssets, 3);
	nodeArray[25] = Node(cdsTransAct, &cubeAssets, 3);
	nodeArray[26] = Node(cdsTransRep, &cubeAssets, 3);
	nodeArray[27] = Node(cdsTransMult, &cubeAssets, 3);

	nodeArray[28] = Node(termEcFor, &cubeAssets, 3);
	nodeArray[29] = Node(termEcRev, &cubeAssets, 3);
	nodeArray[30] = Node(termEcBi, &cubeAssets, 3);

	//setting level 3 children
	Node promEcoliArray[4];
	for (int i = 0; i < 3; i++){
		promEcoliArray[i] = nodeArray[i + 16];
	}
	promEcoliArray[3] = nodeArray[31];
	nodeArray[5].setChildren(promEcoliArray);

	Node promYeastArray[4];
	promYeastArray[0] = nodeArray[19];
	promYeastArray[1] = nodeArray[20];
	promYeastArray[2] = nodeArray[21];
	promYeastArray[3] = nodeArray[22];
	nodeArray[6].setChildren(promYeastArray);

	Node cdsRepArray[2];
	cdsRepArray[0] = nodeArray[23];
	cdsRepArray[1] = nodeArray[24];
	nodeArray[10].setChildren(cdsRepArray);

	Node cdsTransArray[3];
	cdsTransArray[0] = nodeArray[25];
	cdsTransArray[1] = nodeArray[26];
	cdsTransArray[2] = nodeArray[27];
	nodeArray[12].setChildren(cdsTransArray);

	Node termEcArray[3];
	termEcArray[0] = nodeArray[28];
	termEcArray[1] = nodeArray[29];
	termEcArray[2] = nodeArray[30];
	nodeArray[13].setChildren(termEcArray);

	//creating and setting promoter children 
	Node promArray[2];
	promArray[0] = nodeArray[5];
	promArray[1] = nodeArray[6];
	nodeArray[1].setChildren(promArray);

	//creating and setting rbs children
	Node rbsArray[3];
	for (int i = 0; i < 3; i++){
		rbsArray[i] = nodeArray[i+7];
	}
	nodeArray[2].setChildren(rbsArray);

	//creating and setting cds children
	Node cdsArray[3];
	cdsArray[0] = nodeArray[10];
	cdsArray[1] = nodeArray[11];
	cdsArray[2] = nodeArray[12];
	nodeArray[3].setChildren(cdsArray);

	//creating and setting term children
	Node termArray[3];
	for (int i = 0; i < 3; i++){
		termArray[i] = nodeArray[i + 13];
	}
	nodeArray[4].setChildren(termArray);

	//creating & setting top level children
	Node topArray[4];
	for (int i = 0; i < 4; i++){
		LOG("assigning top array");
		topArray[i] = nodeArray[i + 1];
	}
	nodeArray[0].setChildren(topArray);
}


/* MAIN METHOD
contains begin(), initializes the MenuEvent array, 
initializes menus, & contains doit while loop*/
void main(){
	//TESTING CODE FOR TREE: 
	//static Tree treeItems[numTrees];
	//assign_Trees(treeItems);
	//static Tree currentTree[gNumCubes + 1];
	//currentTree[gNumCubes] = treeItems[numTrees - 1];

	assign_Nodes(nodeItems);
	currentNode[gNumCubes] = nodeItems[numNodes - 1];

	begin();

	static EventSensor event;
	event.install();

	struct MenuEvent e;

	for (int i = 0; i < gNumCubesConnected; i++)
	{
		menus[i].init(v[i], &cubeAssets, topItems);
		menus[i].anchor(0);
		currentNode[i] = nodeItems[0];
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