#include <iostream>
using namespace std;

const int WYSOKOSC = 15, SZEROKOSC = 25, X_PTAKA = 5;

class ObiektGry {
public:
    virtual void aktualizuj() = 0;
    virtual void rysuj(char plansza[WYSOKOSC][SZEROKOSC]) = 0;
};

class Ptak : public ObiektGry {
    int wysokosc;
public:
    Ptak() { this->wysokosc = WYSOKOSC / 2; }
    
    void podlec() {
        this->wysokosc += 1;
        if (this->wysokosc >= WYSOKOSC) this->wysokosc = WYSOKOSC - 1;
    }
    
    void lekkoOpadaj() {
        if (this->wysokosc > 0) this->wysokosc--;
    }
    
    int getWysokosc() { return this->wysokosc; }
    
    void aktualizuj() override {
        this->wysokosc -= 1;
        if (this->wysokosc < 0) this->wysokosc = 0;
    }
    
    void rysuj(char plansza[WYSOKOSC][SZEROKOSC]) override {
        plansza[this->wysokosc][X_PTAKA] = '@';
        if (X_PTAKA + 1 < SZEROKOSC) plansza[this->wysokosc][X_PTAKA + 1] = '>';
    }
};

class Przeszkoda : public ObiektGry {
    int pozycjaX, start, przerwa;
    bool punktDodany;
public:
    Przeszkoda(int startX) {
        this->pozycjaX = startX;
        this->start = 4;
        this->przerwa = 6;
        this->punktDodany = false;
    }

    bool kolizja(int wys) {
        return this->pozycjaX == X_PTAKA && 
               (wys < this->start || wys > this->start + this->przerwa);
    }

    bool dajPunkt() {
        if (this->pozycjaX < X_PTAKA && this->pozycjaX >= X_PTAKA - 2 && !this->punktDodany) {
            this->punktDodany = true;
            return true;
        }
        return false;
    }

    void resetuj(int poziom) {
        this->pozycjaX = SZEROKOSC + 5;
        this->punktDodany = false;
        if (poziom > 3 && this->przerwa > 3) this->przerwa--;
        this->start = 2 + (poziom % 5);
    }

    void aktualizuj() override {
        this->pozycjaX--;
    }

    void rysuj(char plansza[WYSOKOSC][SZEROKOSC]) override {
        if (this->pozycjaX >= 0 && this->pozycjaX < SZEROKOSC) {
            for (int i = 0; i < WYSOKOSC; i++) {
                if (i < this->start || i > this->start + this->przerwa)
                    plansza[i][this->pozycjaX] = '#';
            }
            if (this->start > 0)
                plansza[this->start][this->pozycjaX] = '=';
            if (this->start + this->przerwa < WYSOKOSC - 1)
                plansza[this->start + this->przerwa][this->pozycjaX] = '=';
        }
    }
    
    bool pozaPlanszaLewo() {
        return this->pozycjaX < -5;
    }
};

class Punktacja {
    int punkty, poziom;
public:
    Punktacja() { this->punkty = 0; this->poziom = 1; }
    void dodaj() { this->punkty++; if (this->punkty % 3 == 0) this->poziom++; }
    int getPoziom() { return this->poziom; }
    int getPunkty() { return this->punkty; }
    void pokaz() { cout << "Punkty: " << this->punkty << " | Poziom: " << this->poziom << endl; }
};

class Gra {
    Ptak ptak;
    Przeszkoda przeszkoda1, przeszkoda2, przeszkoda3;
    Punktacja punkty;

    void rysuj() {
        char plansza[WYSOKOSC][SZEROKOSC];
        for (int i = 0; i < WYSOKOSC; i++)
            for (int j = 0; j < SZEROKOSC; j++)
                plansza[i][j] = ' ';
    
        for (int j = 0; j < SZEROKOSC; j++)
            plansza[0][j] = '-';

        this->ptak.rysuj(plansza);
        this->przeszkoda1.rysuj(plansza);
        this->przeszkoda2.rysuj(plansza);
        this->przeszkoda3.rysuj(plansza);

        cout << endl << "╔═════════════════════════╗" << endl;
        for (int i = WYSOKOSC - 1; i >= 0; i--) {
            cout << "║";
            for (int j = 0; j < SZEROKOSC; j++) cout << plansza[i][j];
            cout << "║" << endl;
        }
        cout << "╚═════════════════════════╝" << endl;
        this->punkty.pokaz();
    }

    bool sprawdzKoniec() {
        if (this->ptak.getWysokosc() == 0 || this->ptak.getWysokosc() == WYSOKOSC - 1) {
            cout << endl << "╔══════════════════════════╗" << endl;
            cout << "║   KONIEC - Uderzenie!    ║" << endl;
            cout << "║  Twoj wynik: " << this->punkty.getPunkty() << " punktow   ║" << endl;
            cout << "╚══════════════════════════╝" << endl;
            return true;
        }

        if (this->przeszkoda1.kolizja(this->ptak.getWysokosc()) || 
            this->przeszkoda2.kolizja(this->ptak.getWysokosc()) ||
            this->przeszkoda3.kolizja(this->ptak.getWysokosc())) {
            cout << endl << "╔══════════════════════════╗" << endl;
            cout << "║   KONIEC - Zderzenie!    ║" << endl;
            cout << "║  Twoj wynik: " << this->punkty.getPunkty() << " punktow   ║" << endl;
            cout << "╚══════════════════════════╝" << endl;
            return true;
        }
        return false;
    }

    void wykonajRuch(char ruch) {
        if (ruch == 's') this->ptak.podlec();
        else if (ruch == 'n') this->ptak.lekkoOpadaj();
        else this->ptak.aktualizuj();
    }

    void sprawdzPunkty() {
        bool punkt1 = this->przeszkoda1.dajPunkt();
        bool punkt2 = this->przeszkoda2.dajPunkt();
        bool punkt3 = this->przeszkoda3.dajPunkt();
        
        if (punkt1) this->punkty.dodaj();
        if (punkt2) this->punkty.dodaj();
        if (punkt3) this->punkty.dodaj();
        
        if (this->przeszkoda1.pozaPlanszaLewo()) {
            this->przeszkoda1.resetuj(this->punkty.getPoziom());
        }
        if (this->przeszkoda2.pozaPlanszaLewo()) {
            this->przeszkoda2.resetuj(this->punkty.getPoziom());
        }
        if (this->przeszkoda3.pozaPlanszaLewo()) {
            this->przeszkoda3.resetuj(this->punkty.getPoziom());
        }
    }

public:
    Gra() : przeszkoda1(SZEROKOSC - 1), przeszkoda2(SZEROKOSC + 8), przeszkoda3(SZEROKOSC + 16) {}

    int start() {
        cout << endl << "s - podlot | n - lekko | w - wyjscie" << endl;
        while (true) {
            this->rysuj();
            
            char ruch;
            cin >> ruch;
            if (ruch == 'w') return -1;
                       
            for (int k = 0; k < 2; k++) {
                this->wykonajRuch(ruch);
                this->przeszkoda1.aktualizuj();
                this->przeszkoda2.aktualizuj();
                this->przeszkoda3.aktualizuj();
                this->sprawdzPunkty();
                if (this->sprawdzKoniec()) return this->punkty.getPunkty();
            }
        }
    }
};

int main() {
    int highScore = 0;
    int liczbaSmieci = 0;
    
    while (true) {
        cout << endl << "╔═══════════════════════════╗" << endl;
        cout << "║     GRA FLAPPY BIRD       ║" << endl;
        cout << "╠═══════════════════════════╣" << endl;
        cout << "║ High Score: " << highScore << " punktow     ║" << endl;
        cout << "║ Liczba smierci: " << liczbaSmieci << "         ║" << endl;
        cout << "╠═══════════════════════════╣" << endl;
        cout << "║ 1 - Start                 ║" << endl;
        cout << "║ 2 - Wyjscie               ║" << endl;
        cout << "╚═══════════════════════════╝" << endl;
        cout << "Wybor: ";
        
        int wybor;
        cin >> wybor;
        
        if (wybor == 1) { 
            Gra gra;
            int wynik = gra.start();
            
            if (wynik >= 0) {
                liczbaSmieci++;
                if (wynik > highScore) {
                    highScore = wynik;
                    cout << endl << " NOWY REKORD!" << endl;
                }
            }
        }
        else if (wybor == 2) break;
        else cout << "Bledny wybor" << endl;
    }
    
    cout << endl << "Dziekujemy za gre!" << endl;
    cout << "Koncowy High Score: " << highScore << " punktow" << endl;
    cout << "Liczba smierci: " << liczbaSmieci << endl;
    
    return 0;
}
