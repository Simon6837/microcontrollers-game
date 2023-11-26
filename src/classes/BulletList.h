// BulletList.h
#ifndef BULLET_LIST_H
#define BULLET_LIST_H
#include "Bullet.h"

class BulletList
{
private:
    struct Node
    {
        Bullet *bullet;
        Node *next;
    };
    Node *head;

public:
    BulletList();
    void addBullet(Bullet *bullet);
    void updateBullets();
    int8_t getBulletCount();
};

#endif // BULLET_LIST_H
