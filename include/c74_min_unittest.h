/// @file	
///	@ingroup 	minapi
///	@copyright	Copyright (c) 2016, Cycling '74
///	@license	Usage of this file and its contents is governed by the MIT License

#pragma once

#define CATCH_CONFIG_MAIN
#include "c74_min_catch.h"


namespace c74 {
	namespace min {


		/// An wrapper class for RAII instantiation of Min objects in unit tests.
		/// @tparam	min_class_type	The name of your class to test and which extends min::object<>.

		template<class min_class_type>
		class test_wrapper {
		public:

			/// Create a test wrapper instance of your object

			test_wrapper() {
				m_minwrap_obj = wrapper_new<min_class_type>(symbol("dummy"), 0, nullptr);
			}


			/// Destroy the instance

			~test_wrapper() {
				max::object_free(m_minwrap_obj);
			}


			/// Access the instance of your Min object
			/// @return	A reference to your object.

			operator min_class_type&() {
				return m_minwrap_obj->min_object;
			}

		private:
			minwrap<min_class_type>* m_minwrap_obj { nullptr };
		};

	} // namespace min


	namespace max {

		///	A vector of atoms.

		using t_atom_vector = min::atoms;


		/// A sequence of atom vectors.
		/// Used to log inlet and outlet activity in the mock environment.
		/// We can use logging of inlet and outlet sequences for BDD.
		/// We can also do more traditional state-based testing.
		/// Or mix-n-match as we see fit...
		///
		/// @remark		should sequences have time-stamps?

		typedef std::vector<t_atom_vector>	t_sequence;


		// defined in the mock kernel used for executing object unit tests

		extern "C" t_sequence* object_getoutput(void *o, int outletnum);
	
	} // namespace max

} // namespace c74
