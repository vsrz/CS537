// rdtpacket.h
// All the functions and structures necessary to build a single packet

#ifndef RDTPACKET_H
#define RDTPACKET_H

#pragma once

// Size of the data portion of the packet
#define DATA_SIZE 1500

struct pkt {
	uint16_t cksum; /* Ack and Data */
	uint16_t len;   /* Ack and Data */
	uint32_t ackno; /* Ack and Data */
	uint32_t seqno; /* Data only */
	char data[DATA_SIZE]; /* Data only; Not always 1500 bytes, can be less */
};

// Size of the header portion of the packet
const size_t HEADER_SIZE = sizeof( uint16_t ) * 2 + sizeof ( uint32_t ) * 2;

/* Type defines */
typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

/* function prototypes */
uint16_t inet_checksum( unsigned char *, uint32_t );
void setChecksum ( struct pkt * );
pkt *createPacket( pkt , uint32_t , uint32_t , char * , int );
void setPacketSize( pkt *, int );
pkt *buildPacket( pkt *, char * );
pkt *buildAcknowledgementPacket( pkt *, uint32_t  );

uint16_t inet_checksum( unsigned char *addr, uint32_t count )
{
    register uint32_t sum = 0;

    while ( count > 1 )
    {
        sum = sum + *( ( uint16_t * ) addr );
        addr += 2;
        count = count - 2;
    }

    if ( count > 0 )
    {
        sum = sum + *( ( unsigned char * ) addr );
    }

    while ( sum >> 16 )
    {
        sum = ( sum & 0xFFFF ) + ( sum >> 16 );
    }

    return( ~sum );
}

/** 
 *  add the internet checksum to the packet header
 *  setting the cksum value to 00 before calculating 
 *  the checksum.
 *  this should be the last step of the process, before
 *  setting the data portion of the packet
 **/
void setChecksum ( struct pkt *packet )
{	
	pkt p;
	unsigned char header[HEADER_SIZE];
	bzero( &p.cksum, sizeof( uint16_t ));
	p.len = packet->len;	
	p.ackno = packet->ackno;
	p.seqno = packet->seqno;

	memcpy( header, &p, HEADER_SIZE );

	packet->cksum = inet_checksum( header, HEADER_SIZE );

}

/**
 * Places the value of the entire size of the buffer
 * indicated by bufferSize into the packet
 **/
void setPacketSize( pkt *packet, int bufferSize )
{
    packet->len = (uint16_t) bufferSize;
}

/**
 * Places the sequence number into the packet
 **/
void setSequenceNumber( pkt *packet, uint32_t seq )
{
   packet->seqno = seq;
}

/** 
 * Places the acknolwedgement number into the packet
 **/
void setAcknowledgementNumber( pkt *packet, uint32_t ack )
{
   packet->ackno = ack;
}

/** 
 * Creates a packet based on the parameters given
 * Generates the length and cksum independently
 **/
pkt *createPacket( pkt *p, uint32_t ackno, uint32_t seqno, char* data, int dataSize )
{
    setAcknowledgementNumber( p, ackno );
    setSequenceNumber( p, seqno );
    memcpy( p->data, data, dataSize );

    setChecksum( p );

    return p;
}

/**
 * Builds a packet with all the parameters given. Used by the
 * receiver to fill a new packet with received data
 **/
pkt *buildPacket( pkt *p, char *buf )
{
    memcpy( &p->cksum, buf, sizeof(uint16_t) );
    memcpy( &p->len, buf + sizeof(uint16_t), sizeof(uint16_t) );
    memcpy( &p->ackno, buf + sizeof(uint16_t) * 2, sizeof(uint32_t) );
    memcpy( &p->seqno, buf + sizeof(uint16_t) * 2 + sizeof(uint32_t), sizeof(uint32_t) );
    memcpy( p->data, buf + sizeof(uint16_t) * 2 + sizeof(uint32_t) * 2 , p->len ); 
    return p;

}

/**
 * Builds an acknowledgement packet
 **/

pkt *buildAcknowledgementPacket( pkt *p, uint32_t ackno )
{
    p = new pkt;

    bzero( &p, sizeof(struct pkt) );
    memcpy( &p->len, & HEADER_SIZE, sizeof( uint16_t ) );
    memcpy( &p->ackno, &ackno, sizeof( uint32_t ) );
    
}

#endif

