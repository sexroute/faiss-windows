/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD+Patents license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Copyright 2004-present Facebook. All Rights Reserved

#include <cstdio>
#include <cstdlib>

#include <boost/test/unit_test.hpp>
#include "test-util.h"

#include "../IndexIVFPQ.h"
#include "../IndexFlat.h"
#include "../index_io.h"

BOOST_AUTO_TEST_CASE(IVFPQ_index) {

    // dimension of the vectors to index
    int d = 64;

    // size of the database we plan to index
    size_t nb = 1000;

    // make a set of nt training vectors in the unit cube
    // (could be the database)
    size_t nt = 10000;

    // make the index object and train it
    faiss::IndexFlatL2 coarse_quantizer (d);

    // a reasonable number of cetroids to index nb vectors
    int ncentroids = 25;

    faiss::IndexIVFPQ index (&coarse_quantizer, d,
                             ncentroids, 16, 8);

    // index that gives the ground-truth
    faiss::IndexFlatL2 index_gt (d);

    //srand48 (35);
	int nSeed = 35;

    { // training

        std::vector <float> trainvecs (nt * d);
		generate_float_vector(d, trainvecs.data(), nt,nSeed);
        index.verbose = true;
        index.train (nt, trainvecs.data());
    }

    { // populating the database

        std::vector <float> database (nb * d);
		generate_float_vector(d, database.data(), nb, nSeed+1);

        index.add (nb, database.data());
        index_gt.add (nb, database.data());
    }

    int nq = 200;
    int n_ok;

    { // searching the database

        std::vector <float> queries (nq * d);
		generate_float_vector(d, queries.data(), nq, nSeed+2);

        std::vector<faiss::Index::idx_t> gt_nns (nq);
        std::vector<float>               gt_dis (nq);

        index_gt.search (nq, queries.data(), 1,
                         gt_dis.data(), gt_nns.data());

        index.nprobe = 5;
        int k = 5;
        std::vector<faiss::Index::idx_t> nns (k * nq);
        std::vector<float>               dis (k * nq);

        index.search (nq, queries.data(), k, dis.data(), nns.data());

        n_ok = 0;
        for (int q = 0; q < nq; q++) {

            for (int i = 0; i < k; i++)
                if (nns[q * k + i] == gt_nns[q])
                    n_ok++;
        }
		BOOST_TEST_MESSAGE("number of ok " << n_ok);
        BOOST_CHECK(n_ok>nq * 0.4);
    }

}
