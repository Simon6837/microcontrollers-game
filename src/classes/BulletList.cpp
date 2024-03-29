// BulletList.cpp
#include "BulletList.h"
#define SCREEN_TOP_OFFSET 20 // the offset from the top of the screen for deleting bullets
extern const uint8_t maxEnemyRows;
extern const uint8_t maxEnemyColumns;
BulletList::BulletList(bool *playerIsMovingValue, Enemy (*enemiesArray)[5], Score *scoreObject)
{
    playerIsMoving = playerIsMovingValue;
    head = nullptr;
    enemies = enemiesArray;
    score = scoreObject;
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
 * also checks if a bullet has hit a enemy
 * @note Deletes bullets that have gone off the screen
 */
void BulletList::updateBullets()
{
    Node *temp = head;
    Node *prev = nullptr;
    // Loop through all the bullets
    while (temp != nullptr)
    {
        // Check if bullet has hit a enemy
        for (uint8_t j = 0; j < maxEnemyRows; j++)
        {
            for (uint8_t i = 0; i < maxEnemyColumns; i++)
            {
                if (temp->bullet->getXPosition() > enemies[j][i].getXPosition() &&
                    temp->bullet->getXPosition() < enemies[j][i].getXPosition() + 30 &&
                    temp->bullet->getYPosition() > enemies[j][i].getYPosition() &&
                    temp->bullet->getYPosition() < enemies[j][i].getYPosition() + 30)
                {
                    if (enemies[j][i].getType() != 0)
                    {
                        enemies[j][i].setType(0);
                        enemies[j][i].drawEnemy();
                        if (prev == nullptr)
                            head = temp->next;
                        else prev->next = temp->next;
                        delete temp->bullet;
                        delete temp;
                        temp = (prev == nullptr) ? head : prev->next;
                        // increase score
                        score->increaseScore();
                        return;
                    }
                }
            }
        }
        // Check if bullet has gone off the screen
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
        // Update bullet position with respect to player movement
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
/**
 * @brief Deletes all the bullets in the list and the list itself
 */
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
