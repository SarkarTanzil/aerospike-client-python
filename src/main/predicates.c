/*******************************************************************************
 * Copyright 2013-2014 Aerospike, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

/**
 * from aerospike import predicates as p
 *
 * q = client.query(ns,set).where(p.equals("bin",1))
 */

#include <Python.h>
#include <ctype.h>
#include <aerospike/as_query.h>
#include <aerospike/as_error.h>

#include "conversions.h"

static PyObject * AerospikePredicates_Equals(PyObject * self, PyObject * args)
{
	PyObject * py_bin = NULL;
	PyObject * py_val = NULL;

	if ( PyArg_ParseTuple(args, "OO:equals", 
			&py_bin, &py_val) == false ) {
		goto exit;
	}

	if ( PyInt_Check(py_val) || PyLong_Check(py_val) ) {
		return Py_BuildValue("iiOO", AS_PREDICATE_EQUAL, AS_INDEX_NUMERIC, py_bin, py_val);
	}
	else if ( PyString_Check(py_val) || PyUnicode_Check(py_val) ) {
		return Py_BuildValue("iiOO", AS_PREDICATE_EQUAL, AS_INDEX_STRING, py_bin, py_val);
	}

exit:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * AerospikePredicates_Contains(PyObject * self, PyObject * args)
{
	PyObject * py_bin = NULL;
	PyObject * py_val = NULL;
	PyObject * py_datatype = NULL;
	PyObject * py_indextype = NULL;
	int type;
	char *index_type= NULL;

	if ( PyArg_ParseTuple(args, "OOOO:equals", 
			&py_bin, &py_indextype, &py_datatype, &py_val) == false ) {
		goto exit;
	}

	if(PyInt_Check(py_datatype)) {
		type = PyInt_AsLong(py_datatype);
	} else if ( PyLong_Check(py_datatype) ) {
		type = PyLong_AsLongLong(py_datatype);
	}

	if(PyString_Check(py_indextype)) {
		index_type = PyString_AsString(py_indextype);
		for(int i=0;i<=strlen(index_type);i++){
			if(index_type[i]>=97&&index_type[i]<=122)
				index_type[i]=toupper(index_type[i]);
		}
	}

	if ( (PyInt_Check(py_val) || PyLong_Check(py_val)) && type==1 ) {
		return Py_BuildValue("iiOOOs", AS_PREDICATE_EQUAL, AS_INDEX_NUMERIC, py_bin, py_val, Py_None,index_type);
	}
	else if ( (PyString_Check(py_val) || PyUnicode_Check(py_val)) && type == 0) {
		return Py_BuildValue("iiOOOs", AS_PREDICATE_EQUAL, AS_INDEX_STRING, py_bin, py_val, Py_None, index_type);
	}

exit:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * AerospikePredicates_RangeContains(PyObject * self, PyObject * args)
{
	PyObject * py_bin = NULL;
	PyObject * py_min = NULL;
	PyObject * py_max= NULL;
	PyObject * py_datatype = NULL;
	PyObject * py_indextype = NULL;

	if ( PyArg_ParseTuple(args, "OOOOO:equals", 
			&py_bin, &py_indextype, &py_datatype, &py_min, &py_max) == false ) {
		goto exit;
	}

	int type;
	if(PyInt_Check(py_datatype)) {
		type = PyInt_AsLong(py_datatype);
	} else if ( PyLong_Check(py_datatype) ) {
		type = PyLong_AsLongLong(py_datatype);
	}
	char *index_type= NULL;

	if(PyString_Check(py_indextype)) {
		index_type = PyString_AsString(py_indextype);
		for(int i=0;i<=strlen(index_type);i++){
			if(index_type[i]>=97 && index_type[i]<=122)
				index_type[i]=toupper(index_type[i]);
		}
	}
	if ((PyInt_Check(py_min) || PyLong_Check(py_min)) && (PyInt_Check(py_max) || PyLong_Check(py_max)) && type==1 ) {
		return Py_BuildValue("iiOOOs", AS_PREDICATE_RANGE, AS_INDEX_NUMERIC, py_bin, py_min, py_max, index_type);
	}

exit:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * AerospikePredicates_Between(PyObject * self, PyObject * args)
{
	PyObject * py_bin = NULL;
	PyObject * py_min = NULL;
	PyObject * py_max = NULL;

	if ( PyArg_ParseTuple(args, "OOO:between", 
			&py_bin, &py_min, &py_max) == false ) {
		goto exit;
	}

	if ( (PyInt_Check(py_min) || PyLong_Check(py_min)) && (PyInt_Check(py_max) || PyLong_Check(py_max)) ) {
		return Py_BuildValue("iiOOO", AS_PREDICATE_RANGE, AS_INDEX_NUMERIC, py_bin, py_min, py_max);
	}

exit:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef AerospikePredicates_Methods[] = {
	{"equals",		(PyCFunction) AerospikePredicates_Equals,	METH_VARARGS, "Tests whether a bin's value equals the specified value."},
	{"between",		(PyCFunction) AerospikePredicates_Between,	METH_VARARGS, "Tests whether a bin's value is within the specified range."},
	{"contains",	(PyCFunction) AerospikePredicates_Contains,	METH_VARARGS, "Tests whether a bin's value equals the specified value in a complex data type"},
	{"range_contains",	(PyCFunction) AerospikePredicates_RangeContains,	METH_VARARGS, "Tests whether a bin's value is within the specified range in a complex data type"},
	{NULL, NULL, 0, NULL}
};


PyObject * AerospikePredicates_New(void)
{
	PyObject * module = Py_InitModule3("aerospike.predicates", AerospikePredicates_Methods, "Query Predicates");
	return module;
}
