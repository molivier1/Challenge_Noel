#ifndef INVENTAIRE_H
#define INVENTAIRE_H
#include <QDataStream>
class Inventaire
{
public:
    Inventaire();

    int getBle() const;
    void setBle(int newBle);

    int getRoche() const;
    void setRoche(int newRoche);

    int getChene() const;
    void setChene(int newChene);

    int getCarotte() const;
    void setCarotte(int newCarotte);

    int getFer() const;
    void setFer(int newFer);

    int getBouleau() const;
    void setBouleau(int newBouleau);

    int getPatate() const;
    void setPatate(int newPatate);

    int getDiamant() const;
    void setDiamant(int newDiamant);

    int getSapin() const;
    void setSapin(int newSapin);
    friend QDataStream& operator>>(QDataStream& in, Inventaire& v) {
        in >> v.ble >> v.roche >> v.chene >> v.carotte >> v.fer >> v.bouleau >> v.patate >> v.diamant >> v.sapin;
        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const Inventaire& v) {
        out << v.ble << v.roche << v.chene << v.carotte << v.fer << v.bouleau << v.patate << v.diamant << v.sapin;
        return out;
    }

private:
    int ble;
    int roche;
    int chene;
    int carotte;
    int fer;
    int bouleau;
    int patate;
    int diamant;
    int sapin;
};

#endif // INVENTAIRE_H
