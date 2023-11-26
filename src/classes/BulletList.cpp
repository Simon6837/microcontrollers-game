// BulletList.cpp
#include "BulletList.h"

BulletList::BulletList() : head(nullptr) {}
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
        if (temp->bullet->getYPosition() < 10)
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
            temp->bullet->moveUp();
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