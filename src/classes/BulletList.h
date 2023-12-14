// BulletList.h
#ifndef BULLET_LIST_H
#define BULLET_LIST_H
#include "Bullet.h"
#include "NunchukController.h"
#include "Enemy.h"

class BulletList
{
private:
    struct Node
    {
        Bullet *bullet;
        Node *next;
    };
    Node *head;
    bool *playerIsMoving;
    Enemy (*enemies)[5];

public:
    BulletList(bool *playerIsMovingValue, Enemy (*enemiesArray)[5]);
    void addBullet(Bullet *bullet);
    void updateBullets();
    int8_t getBulletCount();
    int16_t getLastButtonYPosition();
    virtual ~BulletList();
};

#endif // BULLET_LIST_H
