/*
 *	gmktrigger.hpp
 *	GMK Trigger
 */

#ifndef __GMK_TRIGGER_HPP
#define __GMK_TRIGGER_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Trigger : public GmkResource
	{
	public:
		enum Moment
		{
			MomentMiddle,
			MomentBegin,
			MomentEnd
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::string			condition;
		unsigned int		momentOfChecking;
		std::string			constantName;

		Trigger(GmkFile* gmk);
		~Trigger();
	};
}

#endif
