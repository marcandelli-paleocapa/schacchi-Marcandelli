#include <iostream>
#include <cstdlib>
using namespace std;

int turno = 1; // 1 = Bianco, 0 = Nero

// VARIABILI GLOBALI PER L'ENPASSANT
bool enPassantAttivo = false;
int enPassantCol = -1;
int enPassantRow = -1;
int enPassantTurno = -1;

// variabili per l'arrocco
bool whiteKingMoved = false, blackKingMoved = false;
bool whiteRookA_moved = false, whiteRookH_moved = false;
bool blackRookA_moved = false, blackRookH_moved = false;

string scacchiera[8][8] = {
    {"♜", "♞", "♝", "♛", "♚", "♝", "♞", "♜"},
    {"♟", "♟", "♟", "♟", "♟", "♟", "♟", "♟"},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {"♙", "♙", "♙", "♙", "♙", "♙", "♙", "♙"},
    {"♖", "♘", "♗", "♕", "♔", "♗", "♘", "♖"}
};

void StampaScacchiera()
{
    for (int i = 0; i < 8; i++)
    {
        cout << 8 - i << "|";
        for (int j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 0)
                cout << "\033[48;5;180m";
            else
                cout << "\033[48;5;94m";

            cout << scacchiera[i][j] << " ";
        }
        cout << "\033[0m\n";
    }
    cout << "  A|B|C|D|E|F|G|H|\n";
}

bool PezzoDelTurno(int col, int row)
{
    if (turno % 2 == 1)
    {
        if (scacchiera[row][col] >= "♔" && scacchiera[row][col] <= "♙")
            return true;
    }
    else
    {
        if (scacchiera[row][col] >= "♚" && scacchiera[row][col] <= "♟")
            return true;
    }
    return false;
}

bool PercorsoLibero(int sc, int sr, int dCol, int dRow)
{
    int stepCol;
    int stepRow;

    if (dCol > 0)
        stepCol = 1;
    else if (dCol < 0)
        stepCol = -1;
    else
        stepCol = 0;

    if (dRow > 0)
        stepRow = 1;
    else if (dRow < 0)
        stepRow = -1;
    else
        stepRow = 0;

    int c = sc + stepCol;
    int r = sr + stepRow;

    while (c != sc + dCol || r != sr + dRow)
    {
        if (scacchiera[r][c] != " ")
            return false;

        c += stepCol;
        r += stepRow;
    }
    return true;
}

// PROTOTIPI necessari perché MossaValida usa queste funzioni definite più in basso
void TrovaRe(bool bianco, int &r, int &c);
bool ReInScacco(bool bianco);
bool MossaSalvaIlRe(int sc, int sr, int ec, int er);

bool MossaValida(int sc, int sr, int ec, int er, bool ignoraAlleati = false)
{
    string p = scacchiera[sr][sc];
    int dCol = ec - sc;
    int dRow = er - sr;
    int Col = abs(dCol);
    int Row = abs(dRow);

    if (Col == 0 && Row == 0)
        return false;

    if (!ignoraAlleati)
    {
        if (scacchiera[er][ec] != " " && PezzoDelTurno(ec, er))
            return false;
    }

    if (p == "♔" || p == "♚")
    {
        // Movimento normale re
        if (Col <= 1 && Row <= 1)
            return true;

        // arrocco movimento di due caselle orizzontali
        if (Row == 0 && Col == 2)
        {
            // Bianco
            if (p == "♔")
            {
                if (whiteKingMoved)
                    return false;

                // arrocco corto (re verso destra)
                if (ec == sc + 2)
                {
                    int rookCol = sc + 3;
                    if (scacchiera[sr][rookCol] != "♖" || whiteRookH_moved)
                        return false;

                    // caselle tra re e torre devono essere libere
                    if (!PercorsoLibero(sc, sr, rookCol - sc, 0))
                        return false;

                    // il re non deve essere sotto scacco ora
                    if (ReInScacco(turno % 2))
                        return false;

                    // le caselle che il re attraversa e quella di arrivo non devono essere sotto attacco
                    if (!MossaSalvaIlRe(sc, sr, sc + 1, sr))
                        return false;
                    if (!MossaSalvaIlRe(sc, sr, sc + 2, sr))
                        return false;

                    return true;
                }

                // arrocco lungo (re verso sinistra)
                if (ec == sc - 2)
                {
                    int rookCol = sc - 4;
                    if (rookCol < 0)
                        return false;
                    if (scacchiera[sr][rookCol] != "♖" || whiteRookA_moved)
                        return false;

                    if (!PercorsoLibero(sc, sr, rookCol - sc, 0))
                        return false;

                    if (ReInScacco(turno % 2))
                        return false;

                    if (!MossaSalvaIlRe(sc, sr, sc - 1, sr))
                        return false;
                    if (!MossaSalvaIlRe(sc, sr, sc - 2, sr))
                        return false;

                    return true;
                }
            }
            else // Nero
            {
                if (blackKingMoved)
                    return false;

                // arrocco corto
                if (ec == sc + 2)
                {
                    int rookCol = sc + 3;
                    if (rookCol > 7)
                        return false;
                    if (scacchiera[sr][rookCol] != "♜" || blackRookH_moved)
                        return false;

                    if (!PercorsoLibero(sc, sr, rookCol - sc, 0))
                        return false;

                    if (ReInScacco(turno % 2))
                        return false;

                    if (!MossaSalvaIlRe(sc, sr, sc + 1, sr))
                        return false;
                    if (!MossaSalvaIlRe(sc, sr, sc + 2, sr))
                        return false;

                    return true;
                }

                // arrocco lungo
                if (ec == sc - 2)
                {
                    int rookCol = sc - 4;
                    if (rookCol < 0)
                        return false;
                    if (scacchiera[sr][rookCol] != "♜" || blackRookA_moved)
                        return false;

                    if (!PercorsoLibero(sc, sr, rookCol - sc, 0))
                        return false;

                    if (ReInScacco(turno % 2))
                        return false;

                    if (!MossaSalvaIlRe(sc, sr, sc - 1, sr))
                        return false;
                    if (!MossaSalvaIlRe(sc, sr, sc - 2, sr))
                        return false;

                    return true;
                }
            }
        }

        return false;
    }

    if (p == "♕" || p == "♛")
    {
        if ((Col == 0 || Row == 0 || Col == Row) &&
            PercorsoLibero(sc, sr, dCol, dRow))
            return true;
        else
            return false;
    }

    if (p == "♖" || p == "♜")
    {
        if ((Col == 0 || Row == 0) &&
            PercorsoLibero(sc, sr, dCol, dRow))
            return true;
        else
            return false;
    }

    if (p == "♗" || p == "♝")
    {
        if (Col == Row && PercorsoLibero(sc, sr, dCol, dRow))
            return true;
        else
            return false;
    }

    if (p == "♘" || p == "♞")
    {
        if ((Col == 2 && Row == 1) ||
            (Col == 1 && Row == 2))
            return true;
        else
            return false;
    }

    if (p == "♙")
    {
        if (dRow == -1 && Col == 0 && scacchiera[er][ec] == " ")
            return true;

        if (sr == 6 && dRow == -2 && Col == 0 &&
            PercorsoLibero(sc, sr, dCol, dRow) &&
            scacchiera[er][ec] == " ")
            return true;

        if (dRow == -1 && Col == 1 &&
            scacchiera[er][ec] != " " && !PezzoDelTurno(ec, er))
            return true;

        // en passant bianco
        if (dRow == -1 && Col == 1 && scacchiera[er][ec] == " ")
        {
            if (enPassantAttivo)
            {
                if (enPassantTurno == turno - 1)
                {
                    if (ec == enPassantCol && sr == enPassantRow)
                        return true;
                }
            }
        }
        return false;
    }

    if (p == "♟")
    {
        if (dRow == 1 && Col == 0 && scacchiera[er][ec] == " ")
            return true;

        if (sr == 1 && dRow == 2 && Col == 0 &&
            PercorsoLibero(sc, sr, dCol, dRow) &&
            scacchiera[er][ec] == " ")
            return true;

        if (dRow == 1 && Col == 1 &&
            scacchiera[er][ec] != " " && !PezzoDelTurno(ec, er))
            return true;

        // EN PASSANT NERO
        if (dRow == 1 && Col == 1 && scacchiera[er][ec] == " ")
        {
            if (enPassantAttivo)
            {
                if (enPassantTurno == turno - 1)
                {
                    if (ec == enPassantCol && sr == enPassantRow)
                        return true;
                }
            }
        }
        return false;
    }

    return false;
}

void TrovaRe(bool bianco, int &r, int &c)
{
    string re = bianco ? "♔" : "♚";

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (scacchiera[i][j] == re)
            {
                r = i;
                c = j;
                return;
            }
}

bool ReInScacco(bool bianco)
{
    int r, c;
    TrovaRe(bianco, r, c);

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            if (scacchiera[i][j] == " ")
                continue;

            if (bianco && scacchiera[i][j] >= "♚" && scacchiera[i][j] <= "♟")
                if (MossaValida(j, i, c, r, true))
                    return true;

            if (!bianco && scacchiera[i][j] >= "♔" && scacchiera[i][j] <= "♙")
                if (MossaValida(j, i, c, r, true))
                    return true;
        }
    return false;
}

bool MossaSalvaIlRe(int sc, int sr, int ec, int er)
{
    string a = scacchiera[sr][sc];
    string b = scacchiera[er][ec];

    scacchiera[er][ec] = a;
    scacchiera[sr][sc] = " ";

    bool sotto = ReInScacco(turno % 2);

    scacchiera[sr][sc] = a;
    scacchiera[er][ec] = b;

    return !sotto;
}

bool ScaccoMatto(bool bianco)
{
    if (!ReInScacco(bianco))
        return false;

    for (int sr = 0; sr < 8; sr++)
        for (int sc = 0; sc < 8; sc++)
        {
            if (scacchiera[sr][sc] == " ")
                continue;

            if (bianco)
            {
                if (!(scacchiera[sr][sc] >= "♔" && scacchiera[sr][sc] <= "♙"))
                    continue;
            }
            else
            {
                if (!(scacchiera[sr][sc] >= "♚" && scacchiera[sr][sc] <= "♟"))
                    continue;
            }

            for (int er = 0; er < 8; er++)
                for (int ec = 0; ec < 8; ec++)
                    if (MossaValida(sc, sr, ec, er))
                        if (MossaSalvaIlRe(sc, sr, ec, er))
                            return false;
        }
    return true;
}

bool EseguiMossa()
{
    string m;
    cin >> m;

    if (m == "resa")
    {
        cout << "RESA! Vince ";
        cout << (turno % 2 ? "NERO\n" : "BIANCO\n");
        exit(0);
    }

    system("clear");

    int sc = m[0] - 'a';
    int sr = 7 - (m[1] - '1');
    int ec = m[2] - 'a';
    int er = 7 - (m[3] - '1');

    if (scacchiera[sr][sc] == " " || !PezzoDelTurno(sc, sr) || !MossaValida(sc, sr, ec, er))
    {
        cout << "MOSSA ILLEGALE\n";
        return false;
    }

    if (!MossaSalvaIlRe(sc, sr, ec, er))
    {
        cout << "MOSSA ILLEGALE: IL RE RESTA SOTTO SCACCO\n";
        return false;
    }

    // RIMOZIONE PEDONE EN PASSANT
    if (scacchiera[sr][sc] == "♙" && ec == enPassantCol && sr == enPassantRow && scacchiera[er][ec] == " ")
        scacchiera[er + 1][ec] = " ";

    if (scacchiera[sr][sc] == "♟" && ec == enPassantCol && sr == enPassantRow && scacchiera[er][ec] == " ")
        scacchiera[er - 1][ec] = " ";

    // esegue la mosssa
    scacchiera[er][ec] = scacchiera[sr][sc];
    scacchiera[sr][sc] = " ";

    // GESTIONE ARROCCO: se il re si è mosso di due colonne, sposta anche la torre
    string movedPiece = scacchiera[er][ec];
    if ((movedPiece == "♔" || movedPiece == "♚") && abs(ec - sc) == 2)
    {
        // Bianco
        if (movedPiece == "♔")
        {
            if (ec == sc + 2) // arrocco corto
            {
                scacchiera[sr][sc + 1] = scacchiera[sr][sc + 3];
                scacchiera[sr][sc + 3] = " ";
                whiteRookH_moved = true;
            }
            else // arrocco lungo
            {
                scacchiera[sr][sc - 1] = scacchiera[sr][sc - 4];
                scacchiera[sr][sc - 4] = " ";
                whiteRookA_moved = true;
            }
            whiteKingMoved = true;
        }
        else // Nero
        {
            if (ec == sc + 2) // arrocco corto
            {
                scacchiera[sr][sc + 1] = scacchiera[sr][sc + 3];
                scacchiera[sr][sc + 3] = " ";
                blackRookH_moved = true;
            }
            else // arrocco lungo
            {
                scacchiera[sr][sc - 1] = scacchiera[sr][sc - 4];
                scacchiera[sr][sc - 4] = " ";
                blackRookA_moved = true;
            }
            blackKingMoved = true;
        }
    }

    // ATTIVAZIONE EN PASSANT
    enPassantAttivo = false;

    if (scacchiera[er][ec] == "♙" && sr == 6 && er == 4)
    {
        enPassantAttivo = true;
        enPassantCol = ec;
        enPassantRow = er;
        enPassantTurno = turno;
    }

    if (scacchiera[er][ec] == "♟" && sr == 1 && er == 3)
    {
        enPassantAttivo = true;
        enPassantCol = ec;
        enPassantRow = er;
        enPassantTurno = turno;
    }

    // Se è stata spostata una torre o un re (mosse normali), aggiorna il bool di muovimento del re di spostamento
    // (queste istruzioni coprono i casi in cui il re/torre si muovono senza arrocco o quando una torre viene catturata)
    if (scacchiera[er][ec] == "♔")
        whiteKingMoved = true;
    if (scacchiera[er][ec] == "♚")
        blackKingMoved = true;

    // rileva se la mossa era lo spostamento di una torre dalle caselle iniziali
    if (scacchiera[er][ec] == "♖")
    {
        if (sr == 7 && sc == 0)
            whiteRookA_moved = true;
        if (sr == 7 && sc == 7)
            whiteRookH_moved = true;
    }
    if (scacchiera[er][ec] == "♜")
    {
        if (sr == 0 && sc == 0)
            blackRookA_moved = true;
        if (sr == 0 && sc == 7)
            blackRookH_moved = true;
    }

    // PROMOZIONE PEDONE
    if (scacchiera[er][ec] == "♙" && er == 0)
    {
        int scelta;
        cout << "Promuovi il pedone bianco in:\n";
        cout << "1. Donna\n2. Torre\n3. Alfiere\n4. Cavallo\n";
        cin >> scelta;

        if (scelta == 1)
        {
            scacchiera[er][ec] = "♕";
        }
        else if (scelta == 2)
        {
            scacchiera[er][ec] = "♖";
        }
        else if (scelta == 3)
        {
            scacchiera[er][ec] = "♗";
        }
        else
        {
            scacchiera[er][ec] = "♘";
        }
    }
    else if (scacchiera[er][ec] == "♟" && er == 7)
    {
        int scelta;
        cout << "Promuovi il pedone nero in:\n";
        cout << "1. Donna\n2. Torre\n3. Alfiere\n4. Cavallo\n";
        cin >> scelta;

        if (scelta == 1)
        {
            scacchiera[er][ec] = "♛";
        }
        else if (scelta == 2)
        {
            scacchiera[er][ec] = "♜";
        }
        else if (scelta == 3)
        {
            scacchiera[er][ec] = "♝";
        }
        else
        {
            scacchiera[er][ec] = "♞";
        }
    }

    return true;
}


int main()
{
system("chcp 65001");
system("clear");

while (true)
{
    StampaScacchiera();

    if (ReInScacco(turno % 2))
        cout << "RE SOTTO SCACCO!\n";

    if (ScaccoMatto(turno % 2))
    {
        cout << "SCACCO MATTO!\nVINCE ";

        if (turno % 2)
            cout << "NERO\n";
        else
            cout << "BIANCO\n";

        break;
    }

    cout << "Turno del ";
    if (turno % 2)
        cout << "BIANCO\n";
    else
        cout << "NERO\n";

    cout << "Inserisci mossa (es: e2e4) o 'resa': ";

    if (EseguiMossa())
        turno++;
}

}
