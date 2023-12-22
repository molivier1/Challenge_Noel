#include "inventaire.h"

Inventaire::Inventaire()
{
    ble = 0;
    roche = 0;
    chene = 0;
    carotte = 0;
    fer = 0;
    bouleau = 0;
    patate = 0;
    diamant = 0;
    sapin = 0;
}

int Inventaire::getBle() const
{
    return ble;
}

void Inventaire::setBle(int newBle)
{
    ble = newBle;
}

int Inventaire::getRoche() const
{
    return roche;
}

void Inventaire::setRoche(int newRoche)
{
    roche = newRoche;
}

int Inventaire::getChene() const
{
    return chene;
}

void Inventaire::setChene(int newChene)
{
    chene = newChene;
}

int Inventaire::getCarotte() const
{
    return carotte;
}

void Inventaire::setCarotte(int newCarotte)
{
    carotte = newCarotte;
}

int Inventaire::getFer() const
{
    return fer;
}

void Inventaire::setFer(int newFer)
{
    fer = newFer;
}

int Inventaire::getBouleau() const
{
    return bouleau;
}

void Inventaire::setBouleau(int newBouleau)
{
    bouleau = newBouleau;
}

int Inventaire::getPatate() const
{
    return patate;
}

void Inventaire::setPatate(int newPatate)
{
    patate = newPatate;
}

int Inventaire::getDiamant() const
{
    return diamant;
}

void Inventaire::setDiamant(int newDiamant)
{
    diamant = newDiamant;
}

int Inventaire::getSapin() const
{
    return sapin;
}

void Inventaire::setSapin(int newSapin)
{
    sapin = newSapin;
}
