#include "Hamming72_64.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

static bool isPowerOf2(uint8_t num);
static int checkBit(uint8_t *const pB, uint8_t pos);
static void flipBit(uint8_t *const pB, uint8_t pos, int bytes);

void set_Hamming72_64(Hamming72_64 *const pH, const uint8_t *const pBits)
{
    memset(pH->bits, 0, sizeof(pH->bits));
    uint8_t pos = 0;
    int parityBitCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int parityBitPos[8] = {0, 1, 2, 4, 8, 16, 32, 64};
    for (int i = 0; i < 8; i++)
    {
        uint8_t temp = pBits[i];
        for (uint8_t j = 0x01; j != 0; j <<= 1)
        {
            while (isPowerOf2(pos))
            {
                pos++;
            }
            if (temp & j)
            {
                flipBit(pH->bits, pos, 8);
                flipBit(pH->bits, 0, 8);
            }
            for (int k = 1; k < 8; k++)
            {
                if ((pos & parityBitPos[k]) && temp & j)
                {
                    parityBitCount[k]++;
                }
            }

            pos++;
        }
    }
    for (int i = 1; i < 8; i++)
    {
        parityBitCount[0] += parityBitCount[i];
    }
    for (int i = 1; i < 8; i++)
    {
        parityBitCount[i] = parityBitCount[i] % 2;
        if (parityBitCount[i] == 1)
        {
            flipBit(pH->bits, 0, 8);
            flipBit(pH->bits, parityBitPos[i], 8);
        }
    }
}

static bool isPowerOf2(uint8_t num)
{
    return (num & (num - 1)) == 0;
}

static void flipBit(uint8_t *const pB, uint8_t pos, int bytes)
{
    pB[pos / bytes] ^= (1 << (pos % 8));
}

static int checkBit(uint8_t *const pB, uint8_t pos)
{
    if ((pB[pos / 8] & (1 << (pos % 8))) == 0)
    {
        return 0;
    }
    return 1;
}

Report_Hamming72_64 correct_Hamming72_64(Hamming72_64 *const pH)
{
    uint8_t oldBits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int pos1 = 0;
    int pos2 = 0;
    int parity = 0;
    for (int i = 0; i < 9; i++)
    {
        uint8_t temp = pH->bits[i];
        for (uint8_t j = 0x01; j != 0; j <<= 1)
        {
            if (!isPowerOf2(pos1))
            {
                if (temp & j)
                {

                    flipBit(oldBits, pos2, 8);
                }

                pos2++;
            }
            if (temp & j)
            {
                parity++;
            }
            pos1++;
        }
    }

    Hamming72_64 *testH = malloc(sizeof(Hamming72_64 *));
    set_Hamming72_64(testH, oldBits);
    int syndrome = 0;
    for (int i = 0; i < 72; i++)
    {
        if (checkBit(testH->bits, i) != checkBit(pH->bits, i))
        {
            if (isPowerOf2(i))
            {
                syndrome += i;
            }
        }
    }
    Report_Hamming72_64 cH;
    cH.status = NOERROR;
    cH.syndrome = syndrome;
    if (parity % 2 == 0 && syndrome != 0)
    {
        cH.status = MULTIBIT;
    }
    else if (parity % 2 == 1)
    {
        if (syndrome == 0)
        {
            cH.status = MASTERPARITY;
        }
        else
        {
            if (isPowerOf2(syndrome))
            {
                cH.status = SINGLEPARITY;
            }
            else
            {
                cH.status = SINGLEDATA;
            }
        }
        flipBit(pH->bits, syndrome, 8);
    }
    free(testH);
    return cH;
}
// On my honor:
//
// - I have not discussed the C language code in my program with
// anyone other than my instructor or the teaching assistants
// assigned to this course.
//
// - I have not used C language code obtained from another student,
// the Internet, or any other unauthorized source, either modified
// or unmodified.
//
// - If any C language code or documentation used in my program
// was obtained from an authorized source, such as a text book or
// course notes, that has been clearly noted with a proper citation
// in the comments of my program.
//
// - I have not designed this program in such a way as to defeat or
// interfere with the normal operation of the grading code.
//
// Jack Gable
// jgable136@vt.edu