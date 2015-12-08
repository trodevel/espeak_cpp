/*

C++ wrapper over ESpeak

Copyright (C) 2015 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Revision: 2930 $ $Date:: 2015-12-07 #$ $Author: serge $


#include "espeak_cpp.h"             // self

#include <espeak/speak_lib.h>

#include "../wave/wave.h"

namespace espeak_cpp
{

ESpeakCpp::ESpeakCpp():
        is_inited_( false ),
        gap_between_words_( 2 ),
        word_rate_( 130 )
{
    char *path = nullptr;
    //int options = espeakINITIALIZE_PHONEME_EVENTS;
    int options = 0;

    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_SYNCHRONOUS;
    int sample_rate = espeak_Initialize( output, 0, path, options );

    if( sample_rate != EE_INTERNAL_ERROR )
    {
        is_inited_ = true;
    }
}

ESpeakCpp::~ESpeakCpp()
{
    if( is_inited_ )
    {
        is_inited_  = false;
        espeak_Terminate();
    }
}

class ESpeakSetter
{
public:

    static void set_sample_rate( ESpeakCpp * self, int rate )
    {
        self->set_sample_rate( rate );
    }

    static void append_samples( ESpeakCpp * self, const char* samples, int size )
    {
        self->append_samples( samples, size );
    }
};

int espeak_callback( short *samples, int numsamples, espeak_EVENT *events )
{
    ESpeakCpp * self = nullptr;

    int type;

    if( samples == NULL )
    {
        return 0;
    }

    if( events )
        self = static_cast<ESpeakCpp*>( events->user_data );

    while( ( type = events->type ) != 0 )
    {
        if( events->type == espeakEVENT_SAMPLERATE )
        {
            ESpeakSetter::set_sample_rate( self, events->id.number );
        }
        events++;
    }

    if( numsamples > 0 )
    {
        ESpeakSetter::append_samples( self, (const char*)samples, numsamples * 2 );
    }

    return 0;
}


bool ESpeakCpp::say( const std::string & text, const std::string & filename, const std::string & voice, std::string & error )
{
    espeak_POSITION_TYPE position_type = POS_CHARACTER;
    void *user_data = this;
    unsigned int position = 0;
    unsigned int end_position = 0;
    unsigned int flags = espeakCHARS_AUTO;
    unsigned int *unique_identifier = nullptr;

    if( is_inited_ == false )
    {
        error = "espeak_Initialize failed";
        return false;
    }

    espeak_SetParameter( espeakWORDGAP, gap_between_words_, 0 );
    espeak_SetParameter( espeakRATE,    word_rate_, 0 );

    espeak_SetSynthCallback( espeak_callback );

    espeak_SetVoiceByName( voice.c_str() );

    espeak_ERROR err2 = espeak_Synth( text.c_str(), text.size(), position,
            position_type, end_position, flags, unique_identifier, user_data );

    if( err2 != EE_OK )
    {
        error = "espeak_Synth failed";
        return false;
    }

    espeak_Synchronize();

    wav_->save( filename );

    return true;
}


void ESpeakCpp::set_gap_between_words( int n )
{
    gap_between_words_  = n;
}

void ESpeakCpp::set_word_rate( int n )
{
    word_rate_  = n;
}

void ESpeakCpp::set_sample_rate( int r )
{
    wav_.reset( new wave::Wave( 1, r, 16 ) );
}

void ESpeakCpp::append_samples( const char* samples, int size )
{
    wav_->append_samples( samples, size );
}

}   // namespace
