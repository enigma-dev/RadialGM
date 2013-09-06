/*
 *	gmktimeline.hpp
 *	GMK Timeline
 */

#ifndef __GMK_TIMELINE_HPP
#define __GMK_TIMELINE_HPP

#include <gmkresource.hpp>
#include <gmkaction.hpp>

namespace Gmk
{
	class Timeline : public GmkResource
	{
	public:
		typedef struct _Moment
		{
			unsigned int			position;
			std::vector<Action*>	actions;
		} Moment;

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::vector<Moment>		moments;

		Timeline(GmkFile* gmk);
		~Timeline();

		int GetId() const;
		void Finalize();
	};
}

#endif
