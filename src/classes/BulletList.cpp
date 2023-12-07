// BulletList.cpp
#include "BulletList.h"
#define SCREEN_TOP_OFFSET 10 // the offset from the top of the screen for deleting bullets
BulletList::BulletList(bool *playerIsMovingValue)
{
    playerIsMoving = playerIsMovingValue;
    head = nullptr;
}
/**
 * @brief Adds a bullet to the list
 */
void BulletList::addBullet(Bullet *bullet)
{
    Node *newNode = new Node;
    newNode->bullet = bullet;
    newNode->next = head;
    head = newNode;
}
/**
 * @brief Updates the position of all the bullets in the list
 * @note Deletes bullets that have gone off the screen
 */
void BulletList::updateBullets()
{
    Node *temp = head;
    Node *prev = nullptr;

    while (temp != nullptr)
    {
        if (temp->bullet->getYPosition() < SCREEN_TOP_OFFSET)
        {
            if (prev == nullptr)
                head = temp->next;
            else
                prev->next = temp->next;
            delete temp->bullet;
            delete temp;
            temp = (prev == nullptr) ? head : prev->next;

            continue;
        }
        else
        {
            uint8_t speed = 0;
            if (playerIsMoving)
                speed = 10;
            else
                speed = 2;
            temp->bullet->moveUp(speed);
        }
        prev = temp;
        temp = temp->next;
    }
}
/**
 * @brief Returns the number of bullets in the list
 */
int8_t BulletList::getBulletCount()
{
    uint8_t count = 0;
    Node *temp = head;
    while (temp != nullptr)
    {
        count++;
        temp = temp->next;
    }
    return count;
}

/**
 * @brief Returns the position of the last bullet in the list
 */
int16_t BulletList::getLastButtonYPosition()
{
    Node *temp = head;
    while (temp != nullptr)
    {
        if (temp->next == nullptr)
            return temp->bullet->getYPosition();
        temp = temp->next;
    }
    return 0;
}

BulletList::~BulletList()
{
    Node *current = head;
    while (current != nullptr)
    {
        Node *next = current->next;
        delete current->bullet;
        delete current;
        current = next;
    }
    head = nullptr;
}
