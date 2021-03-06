#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 1);
		
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;
	
	//倒计时
	time = Label::createWithTTF(ttfConfig, "180");
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);
	//倒计时周期性调用调度器
	dtime = 180;
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(ccp(1, 0));
	pT->setMidpoint(ccp(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	auto frame2 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	attack.pushBack(frame2);
	
	//死亡动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	auto frame3 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 79, 90)));
	dead.pushBack(frame3);
	
	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto frame4 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 68, 101)));
	run.pushBack(frame4);

	//Label
	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel5 = Label::createWithTTF(ttfConfig, "X");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_0(HelloWorld::moveEvent, this,'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_0(HelloWorld::moveEvent, this,'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_0(HelloWorld::moveEvent, this,'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_0(HelloWorld::moveEvent, this,'D'));
	auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_0(HelloWorld::actionEvent, this, 'X'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_0(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width,origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item5->setPosition(Vec2(origin.x+visibleSize.width-item5->getContentSize().width,item1->getPosition().y));
	item6->setPosition(Vec2(item5->getPosition().x-item6->getContentSize().width,item3->getPosition().y));
	
	auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	return true;
}


void HelloWorld::moveEvent(char cid) {

	auto animation = Animation::createWithSpriteFrames(run, 0.08f);
	auto animate = Animate::create(animation);

	if (cid == 'W') {
		/*阻止走出屏幕上边界*/
		if (player->getPositionY() < visibleSize.height) {
			Point p = Point(player->getPositionX(), player->getPositionY() + 30);
			auto move = MoveTo::create(0.4f, p);
			auto myspawn = Spawn::createWithTwoActions(move,animate);
			player->runAction(myspawn);
		}
	}
	if (cid == 'A') {
		if (player->getPositionX() > origin.x) {
			Point p = Point(player->getPositionX() - 30, player->getPositionY());
			auto move = MoveTo::create(0.4f, p);
			auto myspawn = Spawn::createWithTwoActions(move, animate);
			player->runAction(myspawn);
		}
	}
	if (cid == 'S') {
		if (player->getPositionY() > origin.y) {
			Point p = Point(player->getPositionX(), player->getPositionY() - 30);
			auto move = MoveTo::create(0.4f, p);
			auto myspawn = Spawn::createWithTwoActions(move, animate);
			player->runAction(myspawn);
		}
	}
	if (cid == 'D') {
		if (player->getPositionX() < visibleSize.width) {
			Point p = Point(player->getPositionX() + 30, player->getPositionY());
			auto move = MoveTo::create(0.4f, p);
			auto myspawn = Spawn::createWithTwoActions(move, animate);
			player->runAction(myspawn);
		}
	}
}


void HelloWorld::actionEvent(char cid) {

	auto animation2 = Animation::createWithSpriteFrames(dead, 0.1f);
	auto animate2 = Animate::create(animation2);

	auto animation = Animation::createWithSpriteFrames(attack, 0.1f);
	auto animate = Animate::create(animation);

	if (cid == 'X') {
		/*血条减少*/
		pT->setPercentage(pT->getPercentage() - 10);
		player->runAction(animate2);
	}
	if (cid == 'Y') {
		/*血条增加*/
		pT->setPercentage(pT->getPercentage() + 10);
		player->runAction(animate);
	}

}

void HelloWorld::stopAc() {
}

void HelloWorld::updateTime(float dt) {
	/*防止出现负数*/
	if (dtime > 0) {
		dtime--;
		char* mtime = new char[3];
		sprintf(mtime, "%d", dtime);
		time->setString(mtime);
	}

}