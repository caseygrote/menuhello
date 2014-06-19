
#include <sifteo.h>
#include <sifteo/menu.h>
#include "assets.gen.h"


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
//static struct MenuItem promYeConst[] = { { &IconBBa_I766555, &LabelBBa_I766555 }, { NULL, NULL } };
static struct MenuItem promYeNeg[] = { { &IconBBa_K950000, &LabelBBa_K950000 }, { &IconBBa_K950002, &LabelBBa_K950002 }, { NULL, NULL } };
static struct MenuItem promYeMulti[] = { { &IconBBa_I766200, &LabelBBa_I766200 }, { NULL, NULL } };
static struct MenuItem cdsRepChromo[] = {/* { &IconBBa_K59002, &LabelBBa_K59002 }, */{ &IconBBa_K592011, &LabelBBa_K592011 }, { &IconBBa_K592012, &LabelBBa_K592012 }, { NULL, NULL } };
static struct MenuItem cdsRepFluor[] = { { &IconBBa_E0030, &LabelBBa_E0030 }, { &IconBBa_E0020, &LabelBBa_E0020 }, { NULL, NULL } };
//static struct MenuItem cdsTransAct[] = { { &IconBBa_C0079, &LabelBBa_C0079 }, { NULL, NULL } };
static struct MenuItem cdsTransRep[] = { { &IconBBa_C0012, &LabelBBa_C0012 }, { NULL, NULL } };
static struct MenuItem cdsTransMult[] = { { &IconBBa_C0062, &LabelBBa_C0062 }, { NULL, NULL } };
static struct MenuItem termEcFor[] = { { &IconBBa_B0010, &LabelBBa_B0010 }, { NULL, NULL } };
static struct MenuItem termEcRev[] = { { &IconBBa_B0020, &LabelBBa_B0020 }, { NULL, NULL } };
static struct MenuItem termEcBi[] = { { &IconBBa_B0011, &LabelBBa_B0011 }, { &IconBBa_B0014, &LabelBBa_B0014 }, { NULL, NULL } };



static struct MenuAssets gAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };
static struct MenuAssets hAssets = { &BgTile, &Footer, &LabelEmpty, { &Tip0, &Tip1, &Tip2, NULL } };

static Menu menus[gNumCubes];
static VideoBuffer v[gNumCubes];
static struct MenuEvent events[gNumCubes];
static uint8_t cubetouched;

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
				menus[id].init(v[id], &gAssets, topItems); //brings you back to top level @ev
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
	}
}

/*TOSIDE HELPER METHOD: not currently used @ev*/
bool isSide(Side aSide){
	return (aSide == RIGHT || aSide == LEFT);
}

/*NEXT LEVEL HELPER METHOD: not currently used @ev*/
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

/*LEVEL HELPER METHOD
uses tree structure instead of character array*/
void level(unsigned id, PCubeID addedCube, Tree* currentTree){
	LOG("In level method\n");
	unsigned screen = currentScreen[id];
	Tree tr = currentTree[id];
	if (tr.getChildren() == NULL) {
		LOG("NO CHILDREN\n");
		Tree noMore = currentTree[gNumCubes];
		menus[addedCube].init(v[addedCube], noMore.getAssets(), noMore.getMenu());
		currentTree[addedCube] = noMore;
	}
	//else if (tr.getChildren()[screen] == (void*)0){
	//	LOG("no child here\n");
	//	Tree noMore = currentTree[gNumCubes];
	//	menus[addedCube].init(v[addedCube], noMore.getAssets(), noMore.getMenu());
	//	currentTree[addedCube] = noMore;
	//}
	else {
		LOG("we here\n");
		Tree newtr = tr.getChildren()[screen];
		menus[addedCube].init(v[addedCube], newtr.getAssets(), newtr.getMenu());
		currentTree[addedCube] = newtr;
	}
}

/* PLUS CUBE HELPER METHOD
uses tree structure instead of character array; 
fires when cube is neighboured*/
void plusCube(unsigned id, struct MenuEvent e, Tree* currTree){
	LOG("In the plusCube method\n");
	if (e.neighbor.masterSide == BOTTOM && e.neighbor.neighborSide == TOP){
		CubeID(id).detachVideoBuffer();
		PCubeID addedCube = e.neighbor.neighbor;
		level(id, addedCube, currTree);
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
		//nextLevel(addedCube, current);
	}
	//else if (isSide(e.neighbor.masterSide) && isSide(e.neighbor.neighborSide)){
	//	CubeID(id).detachVideoBuffer();
	//}
}

/* DO IT METHOD
handles event cases, takes in Menu and MenuEvent parameters*/
void doit(Menu &m, struct MenuEvent &e, unsigned id, Tree* currTree)
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
			//addCube(m, e, id, currentScreen[id]);
			plusCube(id, e, currTree);
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
			currentScreen[id] = e.item;
			//if statement for current screen array @ev
			//if (e.item == 0){
			//	//currentScreen[id] = 'p';
			//	//LOG("p\n");
			//}
			//else if (e.item == 1){
			//	//currentScreen[id] = 'r';
			//	//LOG("r\n");
			//}
			//else if (e.item == 2){
			//	//currentScreen[id] = 'c';
			//	//LOG("c\n");
			//}
			//else {
			//	//currentScreen[id] = 't';
			//	//LOG("t\n");
			//}
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


/* ASSIGN TREES METHOD
for assigning menus to tree objects*/
void assign_Trees(Tree* treeArray){
	//LEVEL 0
	treeArray[0] = Tree(topItems, &hAssets, 0);
	treeArray[31] = Tree(noItems, &hAssets, 0);

	//LEVEL 1
	treeArray[1] = Tree(promItems, &hAssets, 1);
	treeArray[2] = Tree(rbsItems, &hAssets, 1);
	treeArray[3] = Tree(cdsItems, &hAssets, 1);
	treeArray[4] = Tree(termItems, &hAssets, 1);

	//LEVEL 2
	treeArray[5] = Tree(promEcoliYeast, &hAssets, 2);
	treeArray[6] = Tree(promEcoliYeast, &hAssets, 2);

	treeArray[7] = Tree(rbsConst, &hAssets, 2);
	treeArray[8] = Tree(rbsRibo, &hAssets, 2);
	treeArray[9] = Tree(rbsYeast, &hAssets, 2);

	treeArray[10] = Tree(cdsReport, &hAssets, 2);
	treeArray[11] = Tree(cdsSelect, &hAssets, 2);
	treeArray[12] = Tree(cdsTrans, &hAssets, 2);

	treeArray[13] = Tree(termEcoli, &hAssets, 2);
	treeArray[14] = Tree(termYeast, &hAssets, 2);
	treeArray[15] = Tree(termEuk, &hAssets, 2);

	//LEVEL 3
	treeArray[16] = Tree(promEcPos, &hAssets, 3);
	treeArray[17] = Tree(promEcConst, &hAssets, 3);
	treeArray[18] = Tree(promEcNeg, &hAssets, 3);

	treeArray[19] = Tree(promYePos, &hAssets, 3);
	//treeArray[20] = Tree(promYeConst, &hAssets, 3);
	treeArray[21] = Tree(promYeNeg, &hAssets, 3);
	treeArray[22] = Tree(promYeMulti, &hAssets, 3);

	treeArray[23] = Tree(cdsRepChromo, &hAssets, 3);
	treeArray[24] = Tree(cdsRepFluor, &hAssets, 3);
	//treeArray[25] = Tree(cdsTransAct, &hAssets, 3);
	treeArray[26] = Tree(cdsTransRep, &hAssets, 3);
	treeArray[27] = Tree(cdsTransMult, &hAssets, 3);

	treeArray[28] = Tree(termEcFor, &hAssets, 3);
	treeArray[29] = Tree(termEcRev, &hAssets, 3);
	treeArray[30] = Tree(termEcBi, &hAssets, 3);

	//setting level 3 children
	Tree promEcoliArray[4];
	for (int i = 0; i < 3; i++){
		promEcoliArray[i] = treeArray[i + 16];
	}
	promEcoliArray[3] = treeArray[31];
	treeArray[5].setChildren(promEcoliArray);

	Tree promYeastArray[4];
	promYeastArray[0] = treeArray[19];
	promYeastArray[1] = treeArray[31];
	//promYeastArray[1] = treeArray[20];
	promYeastArray[2] = treeArray[21];
	promYeastArray[3] = treeArray[22];
	treeArray[6].setChildren(promYeastArray);

	Tree cdsRepArray[2];
	cdsRepArray[0] = treeArray[23];
	cdsRepArray[1] = treeArray[24];
	treeArray[10].setChildren(cdsRepArray);

	Tree cdsTransArray[3];
	cdsTransArray[0] = treeArray[31];
	cdsTransArray[1] = treeArray[26];
	cdsTransArray[2] = treeArray[27];
	treeArray[12].setChildren(cdsTransArray);

	Tree termEcArray[3];
	termEcArray[0] = treeArray[28];
	termEcArray[1] = treeArray[29];
	termEcArray[2] = treeArray[30];
	treeArray[13].setChildren(termEcArray);

	//creating and setting promoter children 
	Tree promArray[2];
	promArray[0] = treeArray[5];
	promArray[1] = treeArray[6];
	treeArray[1].setChildren(promArray);

	//creating and setting rbs children
	Tree rbsArray[3];
	for (int i = 0; i < 3; i++){
		rbsArray[i] = treeArray[i+7];
	}
	treeArray[2].setChildren(rbsArray);

	//creating and setting cds children
	Tree cdsArray[3];
	cdsArray[0] = treeArray[10];
	cdsArray[1] = treeArray[11];
	cdsArray[2] = treeArray[12];
	treeArray[3].setChildren(cdsArray);

	//creating and setting term children
	Tree termArray[3];
	for (int i = 0; i < 3; i++){
		termArray[i] = treeArray[i + 13];
	}
	treeArray[4].setChildren(termArray);

	//creating & setting top level children
	Tree topArray[4];
	for (int i = 0; i < 4; i++){
		LOG("assigning top array");
		topArray[i] = treeArray[i + 1];
	}
	treeArray[0].setChildren(topArray);
}


/* MAIN METHOD
contains begin(), initializes the MenuEvent array, 
initializes menus, & contains doit while loop*/
void main(){
	//TESTING CODE FOR TREE: 
	static Tree treeItems[numTrees];
	assign_Trees(treeItems);
	static Tree currentTree[gNumCubes + 1];
	currentTree[gNumCubes] = treeItems[numTrees - 1];

	begin();

	static EventSensor event;
	event.install();

	struct MenuEvent e;

	for (int i = 0; i < gNumCubesConnected; i++)
	{
		menus[i].init(v[i], &gAssets, topItems);
		menus[i].anchor(0);
		currentTree[i] = treeItems[0];
		//currentScreen[i] = 'p'; //proof of concept using char array; why is the string array so hard to invoke im so confused???????? @ev
	}

	while (1){
		
		for (int i = 0; i < gNumCubesConnected; i++){
			doit(menus[i], events[i], i, currentTree);
		}
	}

	ASSERT(e.type == MENU_EXIT);
	//m.performDefault();

	LOG("Selected Game: %d\n", e.item);

}