// BulletList.h
#ifndef BULLET_LIST_H
#define BULLET_LIST_H
#include "Bullet.h"
#include "NunchukController.h"

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

public:
    BulletList(bool *playerIsMovingValue);
    void addBullet(Bullet *bullet);
    void updateBullets();
    int8_t getBulletCount();
    int16_t getLastButtonYPosition();
    virtual ~BulletList();
};

#endif // BULLET_LIST_H
