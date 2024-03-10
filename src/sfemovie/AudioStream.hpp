
/*
 *  AudioStream.hpp
 *  sfeMovie project
 *
 *  Copyright (C) 2010-2014 Lucas Soltic
 *  lucas.soltic@orange.fr
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


#ifndef SFEMOVIE_AUDIOSTREAM_HPP
#define SFEMOVIE_AUDIOSTREAM_HPP

#include <SFML/Audio.hpp>
#include "Stream.hpp"
#include <stdint.h>

namespace sfe {
	class AudioStream : public Stream, private sf::SoundStream {
	public:
		/** Create a video stream from the given FFmpeg stream
		 *
		 * At the end of the constructor, the stream is guaranteed
		 * to have all of its fields set and the decoder loaded
		 */
		AudioStream(AVFormatContextRef formatCtx, AVStreamRef stream, DataSource& dataSource, Timer& timer);
		
		/** Default destructor
		 */
		virtual ~AudioStream();
		
		/** Get the stream kind (either audio, video or subtitle stream)
		 *
		 * @return the kind of stream represented by this stream
		 */
		virtual MediaType getStreamKind() const;
		
		/** Update the stream's status
		 */
		virtual void update();
		
		using sf::SoundStream::setVolume;
		using sf::SoundStream::getVolume;
		using sf::SoundStream::getSampleRate;
		using sf::SoundStream::getChannelCount;
	private:
		virtual bool onGetData(sf::SoundStream::Chunk& data);
		virtual void onSeek(sf::Time timeOffset);
		
		/** Decode the encoded data @a packet into @a outputFrame
		 *
		 * gotFrame being set to false means that decoding should still continue:
		 *  - with a new packet if false is returned
		 *	- with the same packet if true is returned
		 *
		 * @param packet the encoded data
		 * @param outputFrame one decoded data
		 * @param gotFrame set to true if a frame has been extracted to outputFrame, false otherwise
		 * @return true if there's still data to decode in this packet, false otherwise
		 */
		bool decodePacket(AVPacketRef packet, AVFrameRef outputFrame, bool& gotFrame);
		
		/** Initialize the audio resampler for conversion from many formats to signed 16 bits audio
		 *
		 * This must be called before any packet is decoded and resampled
		 */
		void initResampler();
		
		/** Resample the decoded audio frame @a frame into signed 16 bits audio samples
		 *
		 * @param frame the audio samples to convert
		 * @param outSamples [out] the convertedSamples
		 * @param outNbSamples [out] the count of samples in @a outSamples
		 * @param outSamplesLength [out] the length of @a outSamples in bytes
		 */
		void resampleFrame(AVFrameRef frame, uint8_t*& outSamples, int& outNbSamples, int& outSamplesLength);
		
		// Timer::Observer interface
		void willPlay(const Timer &timer);
		void didPlay(const Timer& timer, sfe::Status previousStatus);
		void didPause(const Timer& timer, sfe::Status previousStatus);
		void didStop(const Timer& timer, sfe::Status previousStatus);
		
		// Public properties
		unsigned m_sampleRate;
		
		// Private data
		sf::Int16* m_samplesBuffer;
		AVFrameRef m_audioFrame;
		
		// Resampling
		struct SwrContext* m_swrCtx;
		int m_dstNbSamples;
		int m_maxDstNbSamples;
		int m_dstNbChannels;
		int m_dstLinesize;
		uint8_t** m_dstData;
	};
}

#endif
