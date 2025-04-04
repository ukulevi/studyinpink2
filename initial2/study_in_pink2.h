/*
* Ho Chi Minh City University of Technology
* Faculty of Computer Science and Engineering
* Initial code for Assignment 1
* Programming Fundamentals Spring 2023
* Author: Vu Van Tien
* Date: 02.02.2023
*/

//The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_STUDY_IN_PINK_2_H_
#define _H_STUDY_IN_PINK_2_H_

#include "main.h"
using namespace std;

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class MovingObject;
class Position;
class Configuration;
class Map;

class Criminal;
class RobotS;
class RobotW;
class RobotSW;
class RobotC;

class ArrayMovingObject;
class StudyPinkProgram;

class BaseItem;
class BaseBag;
class SherlockBag;
class WatsonBag;

class TestStudyInPink;

enum ItemType { MAGIC_BOOK, ENERGY_DRINK, FIRST_AID, EXCEMPTION_CARD, PASSING_CARD };
enum ElementType { PATH, WALL, FAKE_WALL };
enum RobotType { C=0, S, W, SW };

class MapElement {
    friend class TestStudyInPink;

protected:
    ElementType type;

public:
    MapElement(ElementType in_type);
    virtual ~MapElement();
    virtual ElementType getType() const;
};

class Path : public MapElement {
    friend class TestStudyInPink;
public:
    Path();
};

class Wall : public MapElement {
    friend class TestStudyInPink;

public:
    Wall();
};

class FakeWall : public MapElement {
    friend class TestStudyInPink;

private:
    int req_exp, r, c;

public:
    FakeWall(int req_exp);
    int getReqEXP() const;
};

class Map {
    friend class TestStudyInPink;

private:
    int num_rows, num_cols;
    MapElement ***map;

public:
    Map(int num_rows, int num_cols, int num_walls, Position * array_walls, int num_fake_walls, Position * array_fake_walls) : num_rows(num_rows), num_cols(num_cols);
    ~Map();
    bool isValid(const Position &pos, MovingObject *mv_obj) const;
    int getNumRow() const { return num_rows; };
    int getNumCol() const { return num_cols; };
};

class Position {
private:
    int r, c;
public:
    static const Position npos;

    Position(int r=0, int c=0);

    Position(const string & str_pos);

    int getRow() const { return r; };
    int getCol() const { return c; };
    void setRow(int r) { this->r = r; };
    void setCol(int c) { this->c = c; };

    string str() const {
        return "(" + to_string(r) + "," + to_string(c) + ")";
    };

    bool isEqual(const Position &pos) const;
    bool isEqual(int in_r, int in_c) const;
};

class MovingObject {
    friend class TestStudyInPink;

protected:
    int index;
    Position pos;
    Map * map;
    string name;
    int hp;
    int exp;

public:
    MovingObject(int index, const Position pos, Map * map, const string & name="");
    virtual ~MovingObject();
    virtual Position getNextPosition() = 0;
    Position getCurrentPosition() const;
    virtual void move() = 0;
    virtual string str() const = 0;
    static const Position npos;
    string getName() const;
    int getHP() const;
    int getEXP() const;
    void setHP(int hp);
    void setEXP(int exp);
};

// Robot, BaseItem, BaseBag,...
class Robot : public MovingObject
{
    friend class TestStudyInPink;

protected:
    RobotType robot_type;
    BaseItem *item;
    Criminal *criminal;

public:
    Robot(int index, const Position &init_pos, Map *map, Criminal *criminal, RobotType robot_type);
    virtual Position getNextPosition() = 0;
    Position getCurrentPosition() const;
    void move();
    virtual BaseItem *getItem();
    static int countRobots;
    static Robot *create(int index, Map *map, Criminal *criminal, Sherlock *sherlock, Watson *watson);
    int getRobotType() const
    {
        return robot_type;
    };
    virtual Position getCriminalPos() const;
    virtual ~Robot();
};

class Character : public MovingObject
{
    friend class TestStudyInPink;

protected:
    string moving_rule;
    BaseBag *bag;
    int steps;

public:
    Character(int index, const Position &init_pos, Map *map, const string &name);
    virtual Position getNextPosition();
    virtual Position getCurrentPosition() const;
    virtual void move();
    virtual string str() const = 0;
    BaseBag *getBag() const;
    int getSteps() const { return steps; };
};

class Sherlock : public Character /* TODO */ {
    friend class TestStudyInPink;

public:
    Sherlock(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp);
    string str() const;
    bool meetRobotC(Robot *robot);
    bool meetRobot(Robot *robot);
    bool meetWatson(Watson *watson);
    bool meetCriminal(Criminal *criminal);
    void move();
};

class Watson : public Character /* TODO */ {
    friend class TestStudyInPink;

public:
    Watson(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp);
    string str() const;
    bool meetRobotC(Robot *robot);
    bool meetRobot(Robot *robot);
    bool meetSherlock(Sherlock *sherlock);
    bool meetCriminal(Criminal *criminal);
    void move();
};

class Criminal : public Character /* TODO */ {
    friend class TestStudyInPink;

private:
    Sherlock *sherlock;
    Watson *watson;
    int countSteps;
    Robot *robot;
    Position prev_pos;
    
public:
    Criminal(int index, const Position &init_pos, Map *map, Sherlock *sherlock, Watson *watson);
    string str() const;
    Position getNextPosition();
    void move();
    int getCountSteps() const
    {
        return countSteps;
    };
    Robot *getRobot() const;
    Position getPrevPos() const;
};

class RobotC : public Robot
{
    friend class TestStudyInPink;

public:
    RobotC(int index, const Position &init_pos, Map *map, Criminal *criminal);
    Position getNextPosition();
    string str() const;
    int getDistance(Character *character) const;
};

class RobotS : public Robot
{
    friend class TestStudyInPink;

private:
    Sherlock *sherlock;
    Criminal *criminal;

public:
    RobotS(int index, const Position &init_pos, Map *map, Criminal *criminal, Sherlock *Sherlock);
    Position getNextPosition();
    string str() const;
    int getDistance() const;
};

class RobotW : public Robot
{
    friend class TestStudyInPink;

private:
    Watson *watson;
    Criminal *criminal;

public:
    RobotW(int index, const Position &init_pos, Map *map, Criminal *criminal, Watson *watson);
    Position getNextPosition();
    string str() const;
    int getDistance() const;
};

class RobotSW : public Robot
{
    friend class TestStudyInPink;

private:
    Criminal *criminal;
    Sherlock *sherlock;
    Watson *watson;

public:
    RobotSW(int index, const Position &init_pos, Map *map, Criminal *criminal, Sherlock *sherlock, Watson *watson);
    Position getNextPosition();
    string str() const;
    int getDistance() const;
};

class ArrayMovingObject {
    friend class TestStudyInPink;

private:
    MovingObject **arr_mv_objs;
    int capacity;
    int count;

public:
    ArrayMovingObject(int capacity);
    ~ArrayMovingObject() ;
    bool isFull() const;
    bool add(MovingObject * mv_obj);
    MovingObject * get(int index) const;
    int size() const; // return current number of elements in the array
    string str() const;
};

class BaseItem
{
    friend class TestStudyInPink;

public:
    BaseItem(){};
    virtual ~BaseItem(){};
    virtual bool canUse(Character *obj, Robot *robot) = 0;
    virtual void use(Character *obj, Robot *robot) = 0;
    virtual ItemType getItemType() const = 0;
};

class MagicBook : public BaseItem
{
    friend class TestStudyInPink;

public:
    bool canUse(Character *obj, Robot *robot);
    void use(Character *obj, Robot *robot);
    ItemType getItemType() const;
};

class EnergyDrink : public BaseItem
{
    friend class TestStudyInPink;

public:
    bool canUse(Character *obj, Robot *robot);
    void use(Character *obj, Robot *robot);
    ItemType getItemType() const;
};

class FirstAid : public BaseItem
{
    friend class TestStudyInPink;

public:
    bool canUse(Character *obj, Robot *robot);
    void use(Character *obj, Robot *robot);
    ItemType getItemType() const;
};

class ExcemptionCard : public BaseItem
{
    friend class TestStudyInPink;

public:
    bool canUse(Character *obj, Robot *robot);
    void use(Character *obj, Robot *robot);
    ItemType getItemType() const;
};

class PassingCard : public BaseItem
{
    friend class TestStudyInPink;

private:
    string challenge;

public:
    PassingCard(const string &challenges);
    bool canUse(Character *obj, Robot *robot);
    void use(Character *obj, Robot *robot);
    string getChallenge();
    ItemType getItemType() const;
};

class BaseBag
{
    friend class TestStudyInPink;

protected:
    class Node
    {
    public:
        BaseItem *item;
        Node *next;

    public:
        Node(BaseItem *item, Node *next = nullptr) : item(item), next(next) {}
    };

protected:
    Node *head;
    Character *character;
    int capacity;
    int countItem;

public:
    BaseBag(int capacity);
    ~BaseBag();
    virtual bool insert(BaseItem *item);
    virtual BaseItem *get();
    virtual BaseItem *get(ItemType itemType);
    virtual string str() const;
    int getCount() const
    {
        return countItem;
    };
};

class WatsonBag : public BaseBag
{
    friend class TestStudyInPink;

private:
    Watson *watson;

public:
    WatsonBag(Watson *watson);
};

class SherlockBag : public BaseBag
{
    friend class TestStudyInPink;

private:
    Sherlock *sherlock;

public:
    SherlockBag(Sherlock *sherlock);
};

class Configuration {
    friend class StudyPinkProgram;

public:
    int map_num_rows, map_num_cols, max_num_moving_objects, num_walls, num_fake_walls, sherlock_init_hp, sherlock_init_exp, watson_init_hp, watson_init_exp, num_steps;
    Position *arr_walls, *arr_fake_walls, sherlock_init_pos, watson_init_pos, criminal_init_pos;
    string sherlock_moving_rule, watson_moving_rule;

    Configuration(const string & filepath);
    ~Configuration();
    string str() const;
};

class StudyPinkProgram {
    friend class Configuration;
    
private:
    // Sample attributes
    Configuration * config;
    
    Sherlock * sherlock;
    Watson * watson;
    Criminal * criminal;
    
    Map * map;
    ArrayMovingObject * arr_mv_objs;


public:
    StudyPinkProgram(const string & config_file_path);

    bool isStop() const;

    void printResult() const {
        if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Sherlock caught the criminal" << endl;
        }
        else if (watson->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
            cout << "Watson caught the criminal" << endl;
        }
        else {
            cout << "The criminal escaped" << endl;
        }
    }

    void printStep(int si) const {
        cout << "Step: " << setw(4) << setfill('0') << si
            << "--"
            << sherlock->str() << "--|--" << watson->str() << "--|--" << criminal->str() << endl;
    }

    void run(bool verbose) {
        // Note: This is a sample code. You can change the implementation as you like.
        // TODO
        for (int istep = 0; istep < config->num_steps; ++istep) {
            for (int i = 0; i < arr_mv_objs->size(); ++i) {
                arr_mv_objs->get(i)->move();
                if (isStop()) {
                    printStep(istep);
                    break;
                }
                if (verbose) {
                    printStep(istep);
                }
            }
        }
        printResult();
    }

    ~StudyPinkProgram();
};

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
#endif /* _H_STUDY_IN_PINK_2_H_ */
