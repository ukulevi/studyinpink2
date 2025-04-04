#include "study_in_pink2.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

MapElement::MapElement(ElementType in_type) {
    this->type = in_type;
}

MapElement::~MapElement() {}

ElementType MapElement::getType() const {
    return type;
}

Path::Path() : MapElement(PATH) {
    type = PATH;
};

Wall::Wall() : MapElement(WALL) {
    type = WALL;
}

FakeWall::FakeWall(int in_req_exp) : MapElement(FAKE_WALL) {
    req_exp = in_req_exp;
}

int FakeWall::getReqEXP() const {
    return req_exp;
}

Map::Map(int num_rows, int num_cols, int num_walls, Position *array_walls, int num_fake_walls, Position *array_fake_walls) : num_rows(num_rows), num_cols(num_cols) {
    map = new MapElement **[num_rows];
    for (int i = 0; i < num_rows; i++) {
        map[i] = new MapElement *[num_cols];
        for (int j = 0; j < num_cols; j++) {
            map[i][j] = new Path();
        }
    }
    for (int i = 0; i < num_walls; i++) {
        map[array_walls[i].getRow()][array_walls[i].getCol()] = new Wall();
    }

    for (int i = 0; i < num_fake_walls; i++) {
        int req_exp = (array_fake_walls[i].getRow() * 257 + array_fake_walls[i].getCol() * 139 + 89) % 900 + 1;
        map[array_fake_walls[i].getRow()][array_fake_walls[i].getCol()] = new FakeWall(req_exp);
    }
}

MovingObject::MovingObject(int index, const Position pos, Map *map, const string &name) {
    this->index = index;
    this->pos = pos;
    this->map = map;
    this->name = name;
}

Position MovingObject::getCurrentPosition() const {
    return pos;
}

const Position Position::npos(-1, -1);

string MovingObject::getName() const {
    return name;
}

void MovingObject::setHP(int hp) {
    this->hp = hp;
}

void MovingObject::setEXP(int exp) {
    this->exp = exp;
}

int MovingObject::getHP() const {
    return hp;
}

int MovingObject::getEXP() const {
    return exp;
}

Map::~Map() {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++){
            delete map[i][j];
        }
        delete[] map[i];
    }
    delete[] map;
}

bool Map::isValid(const Position &pos, MovingObject *mv_obj) const {
    if (pos.getRow() < 0 || pos.getRow() >= num_rows || pos.getCol() < 0 || pos.getCol() >= num_cols) {
        return false;
    }
    if (map[pos.getRow()][pos.getCol()]->getType() == WALL) {
        return false;
    }
    if (map[pos.getRow()][pos.getCol()]->getType() == FAKE_WALL) {
        if (mv_obj->getName() == "Sherlock" || mv_obj->getName() == "Criminal" || mv_obj->getName() == "RobotC" || mv_obj->getName() == "RobotS" || mv_obj->getName() == "RobotW" || mv_obj->getName() == "RobotSW") {
            return true;
        }
        else if (mv_obj->getName() == "Watson") {
            if (mv_obj->getEXP() >= ((FakeWall *)map[pos.getRow()][pos.getCol()])->getReqEXP()) {
                return true;
            }
            return false;
        }
    }
    return true;
}

Position::Position(int r, int c) {
    this->r = r;
    this->c = c;
}

Position::Position(const string &str_pos) {
    sscanf(str_pos.c_str(), "(%d,%d)", &r, &c);
}

bool Position::isEqual(const Position &pos) const {
    if (r == pos.getRow() && c == pos.getCol()) {
        return true;
    }
    return false;
}
bool Position::isEqual(int r, int c) const {
    if (r == this->r && c == this->c) {
        return true;
    }
    return false;
}

Character::Character(int index, const Position &init_pos, Map *map, const string &name) : MovingObject(index, init_pos, map, name) {
    this->steps = 0;
}

Position Character::getCurrentPosition() const {
    return pos;
}

Position Character::getNextPosition() {
    Position next_pos = pos;
    if (moving_rule.length() == 0) {
        next_pos = Position::npos;
    }
    if (steps == moving_rule.length()) {
        steps = 0;
    }
    int temp = 0;
    if (moving_rule[steps] == 'L') {
        next_pos.setCol(pos.getCol() - 1);
        temp += 1;
        steps++;
    }
    else if (moving_rule[steps] == 'R') {
        next_pos.setCol(pos.getCol() + 1);
        temp += 1;
        steps++;
    }
    else if (moving_rule[steps] == 'U') {
        next_pos.setRow(pos.getRow() - 1);
        temp += 1;
        steps++;
    }
    else if (moving_rule[steps] == 'D') {
        next_pos.setRow(pos.getRow() + 1);
        temp += 1;
        steps++;
    }
    if (map->isValid(next_pos, this)) {
        return next_pos;
    }
    else {
        next_pos = pos;
    }
    if (temp == 0) {
        next_pos = Position::npos;
    }
    return next_pos;
}

BaseBag *Character::getBag() const {
    return bag;
}

void Character::move() {
    Position next_pos = getNextPosition();
    if (&next_pos == &Position::npos)
    {
        return;
    }
    pos = next_pos;
}

Sherlock::Sherlock(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp) : Character(index, init_pos, map, "Sherlock") {
    this->moving_rule = moving_rule;
    this->hp = init_hp;
    this->exp = init_exp;
    bag = new SherlockBag(this);
}

string Sherlock::str() const {
    return "Sherlock[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
}

void Sherlock::move() {
    Position next_pos = getNextPosition();
    if (&next_pos == &Position::npos)
    {
        return;
    }
    pos = next_pos;
}

bool Sherlock::meetRobotC(Robot *robot) {
    if (pos.isEqual(robot->getCurrentPosition())) {
        if (this->getEXP() > 500) {
            pos = robot->getCriminalPos();
            robot->~Robot();
        }
        else {
            BaseItem *item = robot->getItem();
            this->bag->insert(item);
            robot->~Robot();
        }
        ItemType item = MAGIC_BOOK;
        BaseItem *magicbook = this->bag->get(item);
        while (magicbook != nullptr && magicbook->canUse(this, robot)) {
            magicbook->use(this, robot);
            magicbook = this->bag->get(item);
        }
        item = FIRST_AID;
        BaseItem *firstaid = this->bag->get(item);
        while (firstaid != nullptr && firstaid->canUse(this, robot)) {
            firstaid->use(this, robot);
            firstaid = this->bag->get(item);
        }
        item = ENERGY_DRINK;
        BaseItem *energydrink = this->bag->get(item);
        while (energydrink != nullptr && energydrink->canUse(this, robot)) {
            energydrink->use(this, robot);
            energydrink = this->bag->get(item);
        }
        return true;
    }
    return false;
}

bool Sherlock::meetRobot(Robot *robot) {
    cout << robot->getName() << endl;
    if (pos.isEqual(robot->getCurrentPosition())) {
        BaseItem *item = this->bag->get(EXCEMPTION_CARD);
        bool Immune = false;
        if ((this->bag->get(EXCEMPTION_CARD) != nullptr && this->bag->get(EXCEMPTION_CARD)->canUse(this, robot))) {
            Immune = true;
        }
        if (robot->getRobotType() == 1) {
            if (this->getEXP() > 400) {
                this->bag->insert(robot->getItem());
                robot->~Robot();
            }
            else {
                if (Immune == false) {
                    this->setEXP(ceil(this->getEXP() * 0.9));
                }
            }
        }
        if (robot->getRobotType() == 2) {
            BaseItem *item = robot->getItem();
            this->bag->insert(item);
            robot->~Robot();
        }
        if (robot->getRobotType() == 3) {
            if (this->getEXP() > 300 && this->getHP() > 335) {
                BaseItem *item = robot->getItem();
                this->bag->insert(item);
                robot->~Robot();
            }
            else {
                if (Immune == false) {
                    this->setEXP(ceil(this->getEXP() * 0.85));
                    this->setHP(ceil(this->getHP() * 0.85));
                }
            }
        }
        BaseItem *magicbook = this->bag->get(MAGIC_BOOK);
        while (magicbook != nullptr && magicbook->canUse(this, robot)) {
            magicbook->use(this, robot);
            magicbook = this->bag->get(MAGIC_BOOK);
        }
        BaseItem *firstaid = this->bag->get(FIRST_AID);
        while (firstaid != nullptr && firstaid->canUse(this, robot)) {
            firstaid->use(this, robot);
            firstaid = this->bag->get(FIRST_AID);
        }
        BaseItem *energydrink = this->bag->get(ENERGY_DRINK);
        while (energydrink != nullptr && energydrink->canUse(this, robot)){
            energydrink->use(this, robot);
            energydrink = this->bag->get(ENERGY_DRINK);
        }
        return true;
    }
    return false;
}
bool Sherlock::meetWatson(Watson *watson)
{
    if (pos.isEqual(watson->getCurrentPosition())){
        ItemType item = PASSING_CARD;
        BaseItem *pcard = this->bag->get(item);
        if (pcard == nullptr || watson->getBag() == nullptr){
            return false;
        }
        while (pcard != nullptr) {
            watson->getBag()->insert(pcard);
            pcard = this->bag->get(item);
            if (pcard == nullptr || watson->getBag() == nullptr)
            {
                break;
            }
        }
        return true;
    }
    return false;
}

Watson::Watson(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp) : Character(index, init_pos, map, "Watson"){
    this->moving_rule = moving_rule;
    this->hp = init_hp;
    this->exp = init_exp;
    bag = new WatsonBag(this);
}

string Watson::str() const {
    return "Watson[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
}

bool Watson::meetSherlock(Sherlock *sherlock) {
    if (pos.isEqual(sherlock->getCurrentPosition()))
    {
        ItemType item = EXCEMPTION_CARD;
        BaseItem *ecard = this->bag->get(item);
        while (ecard != nullptr)
        {
            sherlock->getBag()->insert(ecard);
            ecard = this->bag->get(item);
        }
        return true;
    }
    return false;
}

bool Watson::meetRobotC(Robot *robot){
    if (pos.isEqual(robot->getCurrentPosition()))
    {
        BaseItem *itemGet = robot->getItem();
        this->bag->insert(itemGet);
        robot = nullptr;
        ItemType item = MAGIC_BOOK;
        BaseItem *magicbook = this->bag->get(item);
        while (magicbook != nullptr)
        {
            magicbook->use(this, robot);
            magicbook = this->bag->get(item);
        }
        item = FIRST_AID;
        BaseItem *firstaid = this->bag->get(item);
        while (firstaid != nullptr)
        {
            firstaid->use(this, robot);
            firstaid = this->bag->get(item);
        }
        item = ENERGY_DRINK;
        BaseItem *energydrink = this->bag->get(item);
        while (energydrink != nullptr)
        {
            energydrink->use(this, robot);
            energydrink = this->bag->get(item);
        }
        return true;
    }
    return false;
}

bool Watson::meetRobot(Robot *robot){
    if (pos.isEqual(robot->getCurrentPosition())){
        if (this->bag->get(PASSING_CARD) != nullptr){
            BaseItem *item = this->bag->get(PASSING_CARD);
            item->use(this, robot);
            robot->~Robot();
            return true;
        }
        else{
            if (robot->getRobotType() == 2){
                if (this->getHP() > 350) {
                    BaseItem *item = robot->getItem();
                    this->bag->insert(item);
                    robot->~Robot();
                } else{
                    this->setHP(ceil(this->getHP() * 0.95));
                }
            } if (robot->getRobotType() == 3) {
                if (this->getEXP() > 600 && this->getHP() > 165){
                    BaseItem *item = robot->getItem();
                    this->bag->insert(item);
                    robot->~Robot();
                } else {
                    this->setEXP(ceil(this->getEXP() * 0.85));
                    this->setHP(ceil(this->getHP() * 0.85));
                }
            }
        }
        ItemType item = MAGIC_BOOK;
        BaseItem *magicbook = this->bag->get(item);
        while (magicbook != nullptr && magicbook->canUse(this, robot)){
            magicbook->use(this, robot);
            magicbook = this->bag->get(item);
        }
        item = FIRST_AID;
        BaseItem *firstaid = this->bag->get(item);
        while (firstaid != nullptr && firstaid->canUse(this, robot)){
            firstaid->use(this, robot);
            firstaid = this->bag->get(item);
        }
        item = ENERGY_DRINK;
        BaseItem *energydrink = this->bag->get(item);
        while (energydrink != nullptr && energydrink->canUse(this, robot))
        {
            energydrink->use(this, robot);
            energydrink = this->bag->get(item);
        }
        return true;
    }
    return false;
}

void Watson::move() {
    Position next_pos = getNextPosition();
    if (&next_pos == &Position::npos) {
        return;
    }
    pos = next_pos;
}

string Criminal::str() const{
    return "Criminal[index=" + to_string(index) + ";pos=" + pos.str() + "]";
}

Criminal::Criminal(int index, const Position &init_pos, Map *map, Sherlock *sherlock, Watson *watson) : Character(index, init_pos, map, "Criminal"){
    this->sherlock = sherlock;
    this->watson = watson;
    this->prev_pos = Position::npos;
}

Position Criminal::getNextPosition(){
    Position next_pos = pos;
    prev_pos = pos;
    int max_distance = -1;
    Position arr[4];
    arr[0] = Position(pos.getRow() - 1, pos.getCol());
    arr[1] = Position(pos.getRow(), pos.getCol() - 1);
    arr[2] = Position(pos.getRow() + 1, pos.getCol());
    arr[3] = Position(pos.getRow(), pos.getCol() + 1);
    for (int i = 0; i < 4; i++)
    {
        if (map->isValid(arr[i], this))
        {
            int distance = abs(arr[i].getRow() - sherlock->getCurrentPosition().getRow()) + abs(arr[i].getCol() - sherlock->getCurrentPosition().getCol()) + abs(arr[i].getRow() - watson->getCurrentPosition().getRow()) + abs(arr[i].getCol() - watson->getCurrentPosition().getCol());
            if (distance > max_distance)
            {
                max_distance = distance;
                next_pos = arr[i];
            }
            else if (distance == max_distance)
            {
                continue;
            }
        }
    }
    if (&next_pos == &pos)
    {
        next_pos = Position::npos;
    }
    return next_pos;
}

void Criminal::move(){
    Position next_pos = getNextPosition();
    if (&next_pos == &Position::npos){
        return;
    }
    pos = next_pos;
}

Robot *Criminal::getRobot() const {
    return robot;
}

Position Criminal::getPrevPos() const {
    return prev_pos;
}

Robot::Robot(int index, const Position &init_pos, Map *map, Criminal *criminal, RobotType robot_type) : MovingObject(index, init_pos, map, "Robot")

{
    this->robot_type = robot_type;
    this->criminal = criminal;
};

Robot::~Robot()
{
    delete item;
    delete criminal;
    item = nullptr;
    criminal = nullptr;
}

Position Robot::getCurrentPosition() const
{
    return pos;
}

void Robot::move()
{
    Position next_pos = getNextPosition();
    if (&next_pos == &Position::npos)
    {
        return;
    }
    pos = next_pos;
}

BaseItem *Robot::getItem()
{
    return item;
}

int Robot::countRobots = 0;

Robot *Robot::create(int index, Map *map, Criminal *criminal, Sherlock *sherlock, Watson *watson)
{
    Robot *robot;
    Position pos = criminal->getCurrentPosition();
    if (Robot::countRobots == 0)
    {
        *robot = RobotC(0, pos, map, criminal);
    }
    else
    {
        int distanceS = abs(pos.getRow() - sherlock->getCurrentPosition().getRow()) + abs(pos.getCol() - sherlock->getCurrentPosition().getCol());
        int distanceW = abs(pos.getRow() - watson->getCurrentPosition().getRow()) + abs(pos.getCol() - watson->getCurrentPosition().getCol());
        if (distanceS < distanceW)
        {
            *robot = RobotS(0, pos, map, criminal, sherlock);
        }
        else if (distanceS > distanceW)
        {
            *robot = RobotW(0, pos, map, criminal, watson);
        }
        else
        {
            *robot = RobotSW(0, pos, map, criminal, sherlock, watson);
        }
    }
    return robot;
}
Position Robot::getCriminalPos() const
{
    return criminal->getCurrentPosition();
}

RobotC::RobotC(int index, const Position &init_pos, Map *map, Criminal *criminal) : Robot(index, init_pos, map, criminal, C)
{
    this->name = "RobotC";
    int p = init_pos.getRow() * init_pos.getCol();
    int t = (init_pos.getRow() * 11 + init_pos.getCol()) % 4;
    int s = 0;
    while (p > 0)
    {
        s += p % 10;
        p /= 10;
    }
    while (s >= 10)
    {
        p = s;
        s = 0;
        while (p > 0)
        {
            s += p % 10;
            p /= 10;
        }
    }
    if (s >= 0 && s <= 1)
    {
        *item = MagicBook();
    }
    else if (s >= 2 && s <= 3)
    {
        *item = EnergyDrink();
    }
    else if (s >= 4 && s <= 5)
    {
        *item = FirstAid();
    }
    else if (s >= 6 && s <= 7)
    {
        *item = ExcemptionCard();
    }
    else if (s >= 8 && s <= 9)
    {
        switch (t)
        {
        case 0:
            *item = PassingCard("RobotS");
            break;
        case 1:
            *item = PassingCard("RobotC");
            break;
        case 2:
            *item = PassingCard("RobotSW");
            break;
        case 3:
            *item = PassingCard("all");
            break;

        default:
            break;
        }
    }
};

Position RobotC::getNextPosition()
{
    Position next_pos = criminal->getPrevPos();
    if (&next_pos == &pos)
    {
        next_pos = Position::npos;
    }
    return next_pos;
};

string RobotC::str() const
{
    return "Robot[pos=" + pos.str() + ";type=C" + ";dist=]";
}

int RobotC::getDistance(Character *character) const
{
    return abs(pos.getRow() - character->getCurrentPosition().getRow()) + abs(pos.getCol() - character->getCurrentPosition().getCol());
};

RobotS::RobotS(int index, const Position &init_pos, Map *map, Criminal *criminal, Sherlock *Sherlock) : Robot(index, init_pos, map, criminal, S)
{
    this->sherlock = Sherlock;
    this->name = "RobotS";
    int p = init_pos.getRow() * init_pos.getCol();
    int t = (init_pos.getRow() * 11 + init_pos.getCol()) % 4;
    int s = 0;
    while (p > 0)
    {
        s += p % 10;
        p /= 10;
    }
    while (s >= 10)
    {
        p = s;
        s = 0;
        while (p > 0)
        {
            s += p % 10;
            p /= 10;
        }
    }
    if (s >= 0 && s <= 1)
    {
        item = new MagicBook();
    }
    else if (s >= 2 && s <= 3)
    {
        item = new EnergyDrink();
    }
    else if (s >= 4 && s <= 5)
    {
        item = new FirstAid();
    }
    else if (s >= 6 && s <= 7)
    {
        item = new ExcemptionCard();
    }
    else if (s >= 8 && s <= 9)
    {
        switch (t)
        {
        case 0:
            *item = PassingCard("RobotS");
            break;
        case 1:
            *item = PassingCard("RobotC");
            break;
        case 2:
            *item = PassingCard("RobotSW");
            break;
        case 3:
            *item = PassingCard("all");
            break;

        default:
            break;
        }
    }
};

int RobotS::getDistance() const
{
    return abs(pos.getRow() - sherlock->getCurrentPosition().getRow()) + abs(pos.getCol() - sherlock->getCurrentPosition().getCol());
};

Position RobotS::getNextPosition()
{
    Position next_pos = pos;
    int min_distance = 1000000;
    Position arr[4];
    arr[0] = Position(pos.getRow() - 1, pos.getCol());
    arr[1] = Position(pos.getRow(), pos.getCol() + 1);
    arr[2] = Position(pos.getRow() + 1, pos.getCol());
    arr[3] = Position(pos.getRow(), pos.getCol() - 1);
    for (int i = 0; i < 4; i++)
    {
        if (map->isValid(arr[i], this))
        {
            int distance = abs(arr[i].getRow() - sherlock->getCurrentPosition().getRow()) + abs(arr[i].getCol() - sherlock->getCurrentPosition().getCol());
            if (distance < min_distance)
            {
                min_distance = distance;
                next_pos = arr[i];
            }
            else if (distance == min_distance)
            {
                continue;
            }
        }
    }
    if (&next_pos == &pos)
    {
        next_pos = Position::npos;
    }
    return next_pos;
};

string RobotS::str() const
{
    return "Robot[pos=" + pos.str() + ";type=S" + ";dist=" + to_string(getDistance()) + "]";
}

RobotW::RobotW(int index, const Position &init_pos, Map *map, Criminal *criminal, Watson *watson) : Robot(index, init_pos, map, criminal, W)
{
    this->watson = watson;
    this->name = "RobotW";
    int p = init_pos.getRow() * init_pos.getCol();
    int t = (init_pos.getRow() * 11 + init_pos.getCol()) % 4;
    int s = 0;
    while (p > 0)
    {
        s += p % 10;
        p /= 10;
    }
    while (s >= 10)
    {
        p = s;
        s = 0;
        while (p > 0)
        {
            s += p % 10;
            p /= 10;
        }
    }
    if (s >= 0 && s <= 1)
    {
        *item = MagicBook();
    }
    else if (s >= 2 && s <= 3)
    {
        *item = EnergyDrink();
    }
    else if (s >= 4 && s <= 5)
    {
        *item = FirstAid();
    }
    else if (s >= 6 && s <= 7)
    {
        *item = ExcemptionCard();
    }
    else if (s >= 8 && s <= 9)
    {
        switch (t)
        {
        case 0:
            *item = PassingCard("RobotS");
            break;
        case 1:
            *item = PassingCard("RobotC");
            break;
        case 2:
            *item = PassingCard("RobotSW");
            break;
        case 3:
            *item = PassingCard("all");
            break;

        default:
            break;
        }
    }
};

Position RobotW::getNextPosition()
{
    Position next_pos = pos;
    int min_distance = 1000000;
    Position arr[4];
    arr[0] = Position(pos.getRow() - 1, pos.getCol());
    arr[1] = Position(pos.getRow(), pos.getCol() + 1);
    arr[2] = Position(pos.getRow() + 1, pos.getCol());
    arr[3] = Position(pos.getRow(), pos.getCol() - 1);
    for (int i = 0; i < 4; i++)
    {
        if (map->isValid(arr[i], this))
        {
            int distance = abs(arr[i].getRow() - watson->getCurrentPosition().getRow()) + abs(arr[i].getCol() - watson->getCurrentPosition().getCol());
            if (distance < min_distance)
            {
                min_distance = distance;
                next_pos = arr[i];
            }
            else if (distance == min_distance)
            {
                continue;
            }
        }
    }
    if (&next_pos == &pos)
    {
        next_pos = Position::npos;
    }
    return next_pos;
}

int RobotW::getDistance() const
{
    return abs(pos.getRow() - watson->getCurrentPosition().getRow()) + abs(pos.getCol() - watson->getCurrentPosition().getCol());
};

string RobotW::str() const
{
    return "Robot[pos=" + pos.str() + ";type=W" + ";dist=" + to_string(getDistance()) + "]";
}

RobotSW::RobotSW(int index, const Position &init_pos, Map *map, Criminal *criminal, Sherlock *Sherlock, Watson *watson) : Robot(index, init_pos, map, criminal, SW)
{
    this->sherlock = Sherlock;
    this->watson = watson;
    this->name = "RobotSW";
    int p = init_pos.getRow() * init_pos.getCol();
    int t = (init_pos.getRow() * 11 + init_pos.getCol()) % 4;
    int s = 0;
    while (p > 0)
    {
        s += p % 10;
        p /= 10;
    }
    while (s >= 10)
    {
        p = s;
        s = 0;
        while (p > 0)
        {
            s += p % 10;
            p /= 10;
        }
    }
    if (s >= 0 && s <= 1)
    {
        *item = MagicBook();
    }
    else if (s >= 2 && s <= 3)
    {
        *item = EnergyDrink();
    }
    else if (s >= 4 && s <= 5)
    {
        *item = FirstAid();
    }
    else if (s >= 6 && s <= 7)
    {
        *item = ExcemptionCard();
    }
    else if (s >= 8 && s <= 9)
    {
        switch (t)
        {
        case 0:
            *item = PassingCard("RobotS");
            break;
        case 1:
            *item = PassingCard("RobotC");
            break;
        case 2:
            *item = PassingCard("RobotSW");
            break;
        case 3:
            *item = PassingCard("all");
            break;

        default:
            break;
        }
    }
};

Position RobotSW::getNextPosition()
{
    Position next_pos = pos;
    int min_distance = 1000000;
    Position arr[4];
    arr[0] = Position(pos.getRow() - 2, pos.getCol());
    arr[1] = Position(pos.getRow(), pos.getCol() + 2);
    arr[2] = Position(pos.getRow() + 2, pos.getCol());
    arr[3] = Position(pos.getRow(), pos.getCol() - 2);
    for (int i = 0; i < 4; i++)
    {
        if (map->isValid(arr[i], this))
        {
            int distance = abs(arr[i].getRow() - sherlock->getCurrentPosition().getRow()) + abs(arr[i].getCol() - sherlock->getCurrentPosition().getCol()) + abs(arr[i].getRow() - watson->getCurrentPosition().getRow()) + abs(arr[i].getCol() - watson->getCurrentPosition().getCol());
            if (distance < min_distance)
            {
                min_distance = distance;
                next_pos = arr[i];
            }
            else if (distance == min_distance)
            {
                continue;
            }
        }
    }
    if (&next_pos == &pos)
    {
        next_pos = Position::npos;
    }
    return next_pos;
}

int RobotSW::getDistance() const
{
    return abs(pos.getRow() - sherlock->getCurrentPosition().getRow()) + abs(pos.getCol() - sherlock->getCurrentPosition().getCol()) + abs(pos.getRow() - watson->getCurrentPosition().getRow()) + abs(pos.getCol() - watson->getCurrentPosition().getCol());
};

string RobotSW::str() const
{
    return "Robot[pos=" + pos.str() + ";type=SW" + ";dist=" + to_string(getDistance()) + "]";
}

bool BaseItem::canUse(Character *obj, Robot *robot)
{
    if ((robot->getItem())->canUse(obj, robot))
    {
        return true;
    };
    return false;
}

ArrayMovingObject::ArrayMovingObject(int capacity) {
    this->capacity = capacity;
    this->count = 0;
    arr_mv_objs = new MovingObject *[capacity];
    for (int i = 0; i < capacity; i++) {
        arr_mv_objs[i] = nullptr;
    }
}

ArrayMovingObject::~ArrayMovingObject() {
    if (arr_mv_objs != nullptr) {
        delete[] arr_mv_objs;
        arr_mv_objs = nullptr;
    }
}

bool ArrayMovingObject::isFull() const {
    if (count == capacity) {
        return true;
    }
    return false;
}

bool ArrayMovingObject::add(MovingObject *mv_obj)
{
    if (isFull()) {
        return false;
    }
    arr_mv_objs[count] = mv_obj;
    count++;
    return true;
}

MovingObject *ArrayMovingObject::get(int index) const
{
    if (index < 0 || index >= count)
    {
        return nullptr;
    }
    return arr_mv_objs[index];
}

int ArrayMovingObject::size() const
{
    return count;
}

string ArrayMovingObject::str() const
{
    string res = "ArrayMovingObject[count=" + to_string(count) + ";capacity=" + to_string(capacity) + ";";
    for (int i = 0; i < count; i++)
    {
        res += get(i)->str();
        if (i != count - 1)
        {
            res += ";";
        }
    }
    res += "]";
    return res;
}

Configuration::Configuration(const string &filepath)
{
    ifstream ifs(filepath);
    if (!ifs.is_open())
    {
        exit(1);
    }
    string s;
    for (int i = 0; i < 15 && !ifs.eof(); i++)
    {
        getline(ifs, s);
        string temp;
        if (s.find("MAP_NUM_ROWS") == 0)
        {
            map_num_rows = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("MAP_NUM_COLS") == 0)
        {
            map_num_cols = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("MAX_NUM_MOVING_OBJECTS") == 0)
        {
            max_num_moving_objects = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("NUM_WALLS") == 0)
        {
            num_walls = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("NUM_FAKE_WALLS") == 0)
        {
            num_fake_walls = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("ARRAY_WALLS") == 0)
        {
            int numWall = 0;
            for (int i = 0; i < s.length(); i++)
            {
                if (s[i] == ',')
                {
                    numWall++;
                }
            }
            num_walls = numWall;

            if (num_walls > 0)
            {
                arr_walls = new Position[num_walls];
                string start = s.substr(s.find('[') + 1, s.find(']') - s.find('[') - 1);
                for (int i = 0; i < num_walls; i++)
                {
                    string pair = start.substr(start.find('('), start.find(')') - start.find('(') + 1);
                    arr_walls[i] = Position(pair);
                    start = start.substr(start.find(')') + 1);
                }
            }
        }
        else if (s.find("ARRAY_FAKE_WALLS") == 0)
        {
            int numFakeWall = 0;
            for (int i = 0; i < s.length(); i++)
            {
                if (s[i] == ',')
                {
                    numFakeWall++;
                }
            }
            num_fake_walls = numFakeWall;

            if (num_fake_walls > 0)
            {
                arr_fake_walls = new Position[num_fake_walls];
                string start = s.substr(s.find('[') + 1, s.find(']') - s.find('[') - 1);
                for (int i = 0; i < num_fake_walls; i++)
                {
                    string pair = start.substr(start.find('('), start.find(')') - start.find('(') + 1);
                    arr_fake_walls[i] = Position(pair);
                    start = start.substr(start.find(')') + 1);
                }
            }
        }
        else if (s.find("SHERLOCK_MOVING_RULE") == 0)
        {
            sherlock_moving_rule = s.substr(s.find('=') + 1);
        }
        else if (s.find("SHERLOCK_INIT_POS") == 0)
        {
            sherlock_init_pos = Position(s.substr(s.find('=') + 1));
        }
        else if (s.find("SHERLOCK_INIT_HP") == 0)
        {
            sherlock_init_hp = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("SHERLOCK_INIT_EXP") == 0)
        {
            sherlock_init_exp = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("WATSON_MOVING_RULE") == 0)
        {
            watson_moving_rule = s.substr(s.find('=') + 1);
        }
        else if (s.find("WATSON_INIT_POS") == 0)
        {
            watson_init_pos = Position(s.substr(s.find('=') + 1));
        }
        else if (s.find("WATSON_INIT_HP") == 0)
        {
            watson_init_hp = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("WATSON_INIT_EXP") == 0)
        {
            watson_init_exp = stoi(s.substr(s.find('=') + 1));
        }
        else if (s.find("CRIMINAL_INIT_POS") == 0)
        {
            criminal_init_pos = Position(s.substr(s.find('=') + 1));
        }
        else if (s.find("NUM_STEPS") == 0)
        {
            num_steps = stoi(s.substr(s.find('=') + 1));
        }
    }
}
Configuration::~Configuration()
{
    delete[] arr_walls;
    delete[] arr_fake_walls;
    arr_walls = nullptr;
    arr_fake_walls = nullptr;
}

string Configuration::str() const
{
    string res = "Configuration[\n";
    res += "MAP_NUM_ROWS=" + to_string(map_num_rows) + "\n";
    res += "MAP_NUM_COLS=" + to_string(map_num_cols) + "\n";
    res += "MAX_NUM_MOVING_OBJECTS=" + to_string(max_num_moving_objects) + "\n";
    res += "NUM_WALLS=" + to_string(num_walls) + "\n";
    res += "ARRAY_WALLS=[";
    for (int i = 0; i < num_walls; i++)
    {
        res += arr_walls[i].str();
        if (i != num_walls - 1)
        {
            res += ";";
        }
    }
    res += "]\n";
    res += "NUM_FAKE_WALLS=" + to_string(num_fake_walls) + "\n";
    res += "ARRAY_FAKE_WALLS=[";
    for (int i = 0; i < num_fake_walls; i++)
    {
        res += arr_fake_walls[i].str();
        if (i != num_fake_walls - 1)
        {
            res += ";";
        }
    }
    res += "]\n";
    res += "SHERLOCK_MOVING_RULE=" + sherlock_moving_rule + "\n";
    res += "SHERLOCK_INIT_POS=" + sherlock_init_pos.str() + "\n";
    res += "SHERLOCK_INIT_HP=" + to_string(sherlock_init_hp) + "\n";
    res += "SHERLOCK_INIT_EXP=" + to_string(sherlock_init_exp) + "\n";
    res += "WATSON_MOVING_RULE=" + watson_moving_rule + "\n";
    res += "WATSON_INIT_POS=" + watson_init_pos.str() + "\n";
    res += "WATSON_INIT_HP=" + to_string(watson_init_hp) + "\n";
    res += "WATSON_INIT_EXP=" + to_string(watson_init_exp) + "\n";
    res += "CRIMINAL_INIT_POS=" + criminal_init_pos.str() + "\n";
    res += "NUM_STEPS=" + to_string(num_steps) + "\n";
    res += "]";
    return res;
}



ItemType MagicBook::getItemType() const
{
    return MAGIC_BOOK;
}

ItemType EnergyDrink::getItemType() const
{
    return ENERGY_DRINK;
}

ItemType FirstAid::getItemType() const
{
    return FIRST_AID;
}

ItemType ExcemptionCard::getItemType() const
{
    return EXCEMPTION_CARD;
}

ItemType PassingCard::getItemType() const
{
    return PASSING_CARD;
}

bool MagicBook::canUse(Character *obj, Robot *robot)
{
    if (obj->getEXP() >= 350)
    {
        return true;
    }
    return false;
}

void MagicBook::use(Character *obj, Robot *robot)
{
    if (canUse(obj, robot))
    {
        obj->setEXP(ceil(obj->getEXP() * 1.25));
    }
}

bool EnergyDrink::canUse(Character *obj, Robot *robot)
{
    if (obj->getHP() <= 100)
    {
        return true;
    }
    return false;
}

void EnergyDrink::use(Character *obj, Robot *robot)
{
    if (canUse(obj, robot))
    {
        obj->setHP(ceil(obj->getHP() * 1.2));
    }
}

bool FirstAid::canUse(Character *obj, Robot *robot)
{
    if (obj->getHP() <= 100 || obj->getEXP() <= 350)
    {
        return true;
    }
    return false;
}

void FirstAid::use(Character *obj, Robot *robot)
{
    if (canUse(obj, robot))
    {
        obj->setHP(ceil(obj->getHP() * 1.5));
    }
}

bool ExcemptionCard::canUse(Character *obj, Robot *robot)
{
    if (obj->getName() == "Sherlock" && obj->getHP() % 2 != 0)
    {
        return true;
    }
    return false;
}

void ExcemptionCard::use(Character *obj, Robot *robot)
{
    obj->setEXP(obj->getEXP());
    obj->setHP(obj->getHP());
}

PassingCard::PassingCard(const string &challenges)
{
    this->challenge = challenges;
}

bool PassingCard::canUse(Character *obj, Robot *robot)
{
    if (obj->getName() == "Watson" && obj->getEXP() % 2 != 0)
    {
        return true;
    }
    return false;
}

void PassingCard::use(Character *obj, Robot *robot)
{
    if (challenge == robot->getName() || challenge == "all")
    {
        BaseItem *item = robot->getItem();
        obj->getBag()->insert(item);
    }
    else
    {
        obj->setEXP(ceil(obj->getEXP() - 50));
        BaseItem *item = robot->getItem();
        obj->getBag()->insert(item);
    }
}

BaseBag::BaseBag(int capacity)
{
    head = new Node(nullptr, nullptr);
    this->countItem = 0;
    this->capacity = capacity;
}

BaseBag::~BaseBag()
{
    Node *temp = head;
    while (temp != nullptr)
    {
        head = head->next;
        delete temp;
        temp = head;
    }
    head = nullptr;
}

bool BaseBag::insert(BaseItem *item)
{
    if (character->getName() == "Sherlock")
    {
        if (countItem <= 13)
        {
            Node *newNode = new Node(item, head);
            head = newNode;
            countItem++;
            return true;
        }
    }
    if (character->getName() == "Watson")
    {
        if (countItem <= 15)
        {
            Node *newNode = new Node(item, head);
            head = newNode;
            countItem++;
            return true;
        }
    }
    return false;
};

BaseItem *BaseBag::get()
{
    Node *current = head;
    while (current != nullptr)
    {
        if (current->item->canUse(character, nullptr))
        {
            if (current == head)
            {
                head = current->next;
            }
            else
            {
                Node *previous = head;
                while (previous->next != current)
                {
                    previous = previous->next;
                }
                previous->next = current->next;
            }
            BaseItem *item = current->item;
            delete current;
            countItem--;
            return item;
        }
        current = current->next;
    }
    return nullptr;
}

BaseItem *BaseBag::get(ItemType item_type)
{
    Node *current = head;
    Node *previous = nullptr;
    while (current != nullptr)
    {
        if (current->item != nullptr && current->item->getItemType() == item_type)
        {
            if (previous == nullptr)
            {
                head = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            BaseItem *item = current->item;
            delete current;
            countItem--;
            return item;
        }
        previous = current;
        current = current->next;
    }
    return nullptr;
};

string BaseBag::str() const
{
    string res = "Bag[count=" + to_string(countItem) + ";";
    Node *temp = head;
    while (temp != nullptr)
    {
        res += to_string(temp->item->getItemType());
        if (temp->next != nullptr)
        {
            res += ";";
        }
        temp = temp->next;
    }
    return res + "]";
};

SherlockBag::SherlockBag(Sherlock *sherlock) : BaseBag(13)
{
    this->character = sherlock;
}

WatsonBag::WatsonBag(Watson *watson) : BaseBag(15)
{
    this->character = watson;
}

StudyPinkProgram::StudyPinkProgram(const string &config_file_path)
{
    ifstream ifs(config_file_path);
    if (!ifs.is_open())
    {
        return;
    }
    config = new Configuration(config_file_path);
    cout << config->str() << endl;
    map = new Map(config->map_num_rows, config->map_num_cols, config->num_walls, config->arr_walls, config->num_fake_walls, config->arr_fake_walls);
    sherlock = new Sherlock(1, config->sherlock_moving_rule, config->sherlock_init_pos, map, config->sherlock_init_hp, config->sherlock_init_exp);
    watson = new Watson(2, config->watson_moving_rule, config->watson_init_pos, map, config->watson_init_hp, config->watson_init_exp);
    criminal = new Criminal(0, config->criminal_init_pos, map, sherlock, watson);
    arr_mv_objs = new ArrayMovingObject(config->max_num_moving_objects);
    arr_mv_objs->add(criminal);
    arr_mv_objs->add(sherlock);
    arr_mv_objs->add(watson);
    ifs.close();
}

bool StudyPinkProgram::isStop() const
{
    if (sherlock->getHP() == 1 || watson->getHP() == 1 || criminal->getCurrentPosition().isEqual(sherlock->getCurrentPosition()) || criminal->getCurrentPosition().isEqual(watson->getCurrentPosition()))
    {
        return true;
    }
    return false;
}

bool StudyPinkProgram::checkMeet(int i)
{
    MovingObject *current = arr_mv_objs->get(i);
    int count = arr_mv_objs->size();
    if (current == NULL || i >= count)
        return false;
    if (current->getName() == "Sherlock")
    {
        Sherlock *sherlock = dynamic_cast<Sherlock *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j) // skip itself
                continue;
            if (arr_mv_objs->get(j)->getName() == "Watson" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()))
            {
                Watson *watson = dynamic_cast<Watson *>(arr_mv_objs->get(j));
                sherlock->meetWatson(watson);
            }
            else if (arr_mv_objs->get(j)->getName() == "Criminal" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()))
            {
                return true;
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotC" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()))
            {
                sherlock->meetRobotC(dynamic_cast<RobotC *>(arr_mv_objs->get(i)));
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotS" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()))
            {
                sherlock->meetRobot(dynamic_cast<RobotS *>(arr_mv_objs->get(j)));
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotW" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()))
            {
                sherlock->meetRobot(dynamic_cast<RobotW *>(arr_mv_objs->get(j)));
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotSW" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(sherlock->getCurrentPosition()))
            {
                sherlock->meetRobot(dynamic_cast<RobotSW *>(arr_mv_objs->get(j)));
            }
        }
    }
    else if (current->getName() == "Watson")
    {
        Watson *watson = dynamic_cast<Watson *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j) // skip itself
                continue;
            if (arr_mv_objs->get(j)->getName() == "Sherlock" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(watson->getCurrentPosition()))
            {
                Sherlock *sherlock = dynamic_cast<Sherlock *>(arr_mv_objs->get(j));
                watson->meetSherlock(sherlock);
            }
            else if (arr_mv_objs->get(j)->getName() == "Criminal" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(watson->getCurrentPosition()))
            {
                return true;
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotC" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(watson->getCurrentPosition()))
            {
                watson->meetRobotC(dynamic_cast<RobotC *>(arr_mv_objs->get(j)));
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotS" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(watson->getCurrentPosition()))
            {
                watson->meetRobot(dynamic_cast<RobotS *>(arr_mv_objs->get(j)));
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotW" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(watson->getCurrentPosition()))
            {
                watson->meetRobot(dynamic_cast<RobotW *>(arr_mv_objs->get(j)));
            }
            else if (arr_mv_objs->get(j)->getName() == "RobotSW" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(watson->getCurrentPosition()))
            {
                watson->meetRobot(dynamic_cast<RobotSW *>(arr_mv_objs->get(j)));
            }
        }
    }
    else if (current->getName() == "Criminal")
    {
        Criminal *criminal = dynamic_cast<Criminal *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            if ((arr_mv_objs->get(j)->getName() == "Sherlock" || arr_mv_objs->get(j)->getName() == "Watson") && arr_mv_objs->get(j)->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
                return true;
            }
        }
    }
    else if (current->getName() == "RobotC")
    {
        RobotC *robotC = dynamic_cast<RobotC *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            if (arr_mv_objs->get(j)->getName() == "Sherlock" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotC->getCurrentPosition()))
            {
                Sherlock *sherlock = dynamic_cast<Sherlock *>(arr_mv_objs->get(j));
                sherlock->meetRobotC(robotC);
            }
            if (arr_mv_objs->get(j)->getName() == "Watson" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotC->getCurrentPosition()))
            {
                Watson *watson = dynamic_cast<Watson *>(arr_mv_objs->get(j));
                watson->meetRobot(robotC);
            }
        }
    }
    else if (current->getName() == "RobotS")
    {
        RobotS *robotS = dynamic_cast<RobotS *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            if (arr_mv_objs->get(j)->getName() == "Sherlock" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotS->getCurrentPosition()))
            {
                Sherlock *sherlock = dynamic_cast<Sherlock *>(arr_mv_objs->get(j));
                sherlock->meetRobot(robotS);
            }
            if (arr_mv_objs->get(j)->getName() == "Watson" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotS->getCurrentPosition()))
            {
                Watson *watson = dynamic_cast<Watson *>(arr_mv_objs->get(j));
                watson->meetRobot(robotS);
            }
        }
    }
    else if (current->getName() == "RobotW")
    {
        RobotW *robotW = dynamic_cast<RobotW *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            if (arr_mv_objs->get(j)->getName() == "Sherlock" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotW->getCurrentPosition()))
            {
                Sherlock *sherlock = dynamic_cast<Sherlock *>(arr_mv_objs->get(j));
                sherlock->meetRobot(robotW);
            }
            if (arr_mv_objs->get(j)->getName() == "Watson" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotW->getCurrentPosition()))
            {
                Watson *watson = dynamic_cast<Watson *>(arr_mv_objs->get(j));
                watson->meetRobot(robotW);
            }
        }
    }
    else if (current->getName() == "RobotSW")
    {
        RobotSW *robotSW = dynamic_cast<RobotSW *>(current);
        for (int j = 0; j < count; j++)
        {
            if (i == j)
                continue;
            if (arr_mv_objs->get(j)->getName() == "Sherlock" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotSW->getCurrentPosition()))
            {
                Sherlock *sherlock = dynamic_cast<Sherlock *>(arr_mv_objs->get(j));
                sherlock->meetRobot(robotSW);
            }
            if (arr_mv_objs->get(j)->getName() == "Watson" && arr_mv_objs->get(j)->getCurrentPosition().isEqual(robotSW->getCurrentPosition()))
            {
                Watson *watson = dynamic_cast<Watson *>(arr_mv_objs->get(j));
                watson->meetRobot(robotSW);
            }
        }
    }
    return false;
};

void StudyPinkProgram::run(bool verbose)
{
    for (int istep = 0; istep < config->num_steps; ++istep)
    {
        for (int i = 0; i < arr_mv_objs->size(); ++i)
        {
            arr_mv_objs->get(i)->move();
            int countSteps = 0;
            checkMeet(i);
            cout << arr_mv_objs->get(i)->str() << endl;
            if (isStop())
            {
                printStep(istep);
                printResult();
                return;
            }
            if (arr_mv_objs->get(i)->getName() == "Criminal")
            {
                Robot *robot;
                if (countSteps % 3 == 0 && countSteps != 0)
                {
                    Robot *robot = Robot::create(3 + Robot::countRobots, map, criminal, sherlock, watson);
                }
                countSteps++;
            }
            if (verbose)
            {
                printStep(istep);
            }
        }
    }
    printResult();
}

void StudyPinkProgram::printResult() const
{
    if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition()))
    {
        cout << "Sherlock caught the criminal" << endl;
    }
    else if (watson->getCurrentPosition().isEqual(criminal->getCurrentPosition()))
    {
        cout << "Watson caught the criminal" << endl;
    }
    else
    {
        cout << "The criminal escaped" << endl;
    }
}

void StudyPinkProgram::printStep(int si) const
{
    cout << "Step: " << setw(4) << setfill('0') << si
         << "--"
         << sherlock->str() << "--|--" << watson->str() << "--|--" << criminal->str() << endl;
}
void StudyPinkProgram::run(bool verbose, ofstream &OUTPUT)
{
    for (int istep = 0; istep < config->num_steps; ++istep)
    {
        for (int i = 0; i < arr_mv_objs->size(); ++i)
        {
            arr_mv_objs->get(i)->move();
            int countSteps = 0;
            checkMeet(i);
            cout << arr_mv_objs->get(i)->str() << endl;
            if (isStop())
            {
                printStep(istep);
                printResult();
                return;
            }
            if (arr_mv_objs->get(i)->getName() == "Criminal")
            {
                Robot *robot;
                if (countSteps % 3 == 0 && countSteps != 0)
                {
                    Robot *robot = Robot::create(3 + Robot::countRobots, map, criminal, sherlock, watson);
                }
                countSteps++;
            }
            if (verbose)
            {
                printInfo(istep, i, OUTPUT);
            }
        }
    }
    printResult();
}
void StudyPinkProgram::printInfo(int si, int i, ofstream &OUTPUT)
{
    OUTPUT << endl
           << "*************AFTER MOVE*************" << endl;
    OUTPUT
        << "ROUND : " << si << " - TURN : " << i << endl;
    stringstream ss(arr_mv_objs->str());
    string lineArr = "";
    getline(ss, lineArr, 'C');
    OUTPUT << lineArr << "]" << endl;
    getline(ss, lineArr, ']');
    OUTPUT << "\tC" << lineArr << "]" << endl;
    while (getline(ss, lineArr, ']'))
    {
        if (lineArr.length() > 0)
            OUTPUT << "\t" << lineArr.substr(1) << "]" << endl;
    }
    OUTPUT << "Sherlock HP" << sherlock->getHP() << " EXP" << sherlock->getEXP() << endl
           << "Watson HP" << watson->getHP() << " EXP_" << watson->getEXP() << endl
           << "SherlockBag : " << sherlock->getBag()->str() << endl
           << "WatsonBag : " << watson->getBag()->str() << endl;
}
StudyPinkProgram::~StudyPinkProgram()
{
    delete map;
    delete sherlock;
    delete watson;
    delete criminal;
    delete arr_mv_objs;
    delete config;
    map = nullptr;
    sherlock = nullptr;
    watson = nullptr;
    criminal = nullptr;
    arr_mv_objs = nullptr;
    config = nullptr;
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////