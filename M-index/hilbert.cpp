#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hilbert.h"

unsigned* ENCODE ( unsigned* hcode, const unsigned* const point, int DIMS ) {
    unsigned mask = ( unsigned ) 1 << WORDBITS - 1, element, temp1, temp2,
             A, W = 0, S, tS, T, tT, J, P = 0, xJ;
    int	i = NUMBITS * DIMS - DIMS, j;

    memset ( hcode, 0, sizeof ( unsigned ) * DIMS );

    for ( j = A = 0; j < DIMS; j++ )
        if ( point[j] & mask )
            A |= ( 1 << ( DIMS-1-j ) );

    S = tS = A;

    P |= S & ( 1 << ( DIMS-1 ) );
    for ( j = 1; j < DIMS; j++ )
        if ( S & 1 << ( DIMS-1-j ) ^ ( P >> 1 ) & ( 1 << ( DIMS-1-j ) ) )
            P |= ( 1 << ( DIMS-1-j ) );

    /* add in DIMS bits to hcode */
    element = i / WORDBITS;
    if ( i % WORDBITS > WORDBITS - DIMS ) {
        hcode[element] |= P << i % WORDBITS;
        hcode[element + 1] |= P >> WORDBITS - i % WORDBITS;
    } else
        hcode[element] |= P << i - element * WORDBITS;

    J = DIMS;
    for ( j = 1; j < DIMS; j++ )
        if ( ( P >> j & 1 ) == ( P & 1 ) )
            continue;
        else
            break;
    if ( j != DIMS )
        J -= j;
    xJ = J - 1;

    if ( P < 3 )
        T = 0;
    else if ( P % 2 )
        T = ( P - 1 ) ^ ( P - 1 ) / 2;
    else
        T = ( P - 2 ) ^ ( P - 2 ) / 2;
    tT = T;


    for ( i -= DIMS, mask >>= 1; i >=0; i -= DIMS, mask >>= 1 ) {
        for ( j = A = 0; j < DIMS; j++ )
            if ( point[j] & mask )
                A |= ( 1 << ( DIMS-1-j ) );

        W ^= tT;
        tS = A ^ W;
        if ( xJ % DIMS != 0 ) {
            temp1 = tS << xJ % DIMS;
            temp2 = tS >> DIMS - xJ % DIMS;
            S = temp1 | temp2;
            S &= ( ( unsigned ) 1 << DIMS ) - 1;
        } else
            S = tS;

        P = S & ( 1 << ( DIMS-1 ) );
        for ( j = 1; j < DIMS; j++ )
            if ( S & ( 1 << ( DIMS-1-j ) ) ^ ( P >> 1 ) & ( 1 << ( DIMS-1-j ) ) )
                P |= ( 1 << ( DIMS-1-j ) );

        /* add in DIMS bits to hcode */
        element = i / WORDBITS;
        if ( i % WORDBITS > WORDBITS - DIMS ) {
            hcode[element] |= P << i % WORDBITS;
            hcode[element + 1] |= P >> WORDBITS - i % WORDBITS;
        } else
            hcode[element] |= P << i - element * WORDBITS;

        if ( i > 0 ) {
            if ( P < 3 )
                T = 0;
            else if ( P % 2 )
                T = ( P - 1 ) ^ ( P - 1 ) / 2;
            else
                T = ( P - 2 ) ^ ( P - 2 ) / 2;

            if ( xJ % DIMS != 0 ) {
                temp1 = T >> xJ % DIMS;
                temp2 = T << DIMS - xJ % DIMS;
                tT = temp1 | temp2;
                tT &= ( ( unsigned ) 1 << DIMS ) - 1;
            } else
                tT = T;

            J = DIMS;
            for ( j = 1; j < DIMS; j++ )
                if ( ( P >> j & 1 ) == ( P & 1 ) )
                    continue;
                else
                    break;
            if ( j != DIMS )
                J -= j;

            xJ += J - 1;
           
        }
    }
    return hcode;
}