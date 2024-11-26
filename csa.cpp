#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <iostream>

namespace py = pybind11;

// Global variable for the interpreter guard
py::scoped_interpreter guard{};


extern "C" 
void csa_codes_wrapper(double err_un[31],     // input
                       double err_isb[7],     // input
                       double err_lsb0,       // input
                       double err_lsb1,       // input
                       double err_lsb2,       // input
                       double err_lsb3,       // input
                       double err_lsb4,       // input
                       double err_lsb5,       // input
                       double err_extlsb,     // input
                       int    fine_codes[31],         // output
                       int    result_meas_coarse[16], // output
                       int  * cross_meas_coarse,      // output
                       int  * residual_curr_coarse    // output
                      )
{
  py::gil_scoped_acquire acquire;
  py::module_ mod = py::module_::import("csa_module");


  // Convert input C++ arrays to a NumPy arrays
  py::array_t<double>   np_err_un(31, err_un);
  py::array_t<double>   np_err_isb(7,  err_isb);


  // Call the Python function and collect the return value
  py::tuple pytuple = mod.attr("csa_codes")(
                        np_err_un,
                        np_err_isb,
                        err_lsb0,
                        err_lsb1,
                        err_lsb2,
                        err_lsb3,
                        err_lsb4,
                        err_lsb5,
                        err_extlsb
                      );

  // Cast each item to the appropriate type
  py::array_t<double> np_d_fines_codes = pytuple[0].cast<py::array_t<double>>();
  py::array_t<double> np_d_meas_coarse = pytuple[1].cast<py::array_t<double>>();

  py::array_t<int>      np_fines_codes = np_d_fines_codes.attr("astype")("int32");
  py::array_t<int>      np_meas_coarse = np_d_meas_coarse.attr("astype")("int32");

  std::memcpy(fine_codes, np_fines_codes.data(), np_fines_codes.size() * sizeof(int));
  std::memcpy(result_meas_coarse, np_meas_coarse.data(), np_meas_coarse.size() * sizeof(int));

               *cross_meas_coarse = static_cast<int>(pytuple[2].cast<double>());
            *residual_curr_coarse = static_cast<int>(pytuple[3].cast<double>());
}

