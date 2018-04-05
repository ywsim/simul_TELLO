// Copyright (C) 2016, Gurobi Optimization, Inc.
// All Rights Reserved
#ifndef _PARPRIVATE_CPP_H_
#define _PARPRIVATE_CPP_H_

// orders need to match enums in public.h

static const char* dparname[] = {
  "Cutoff",
  "IterationLimit",
  "NodeLimit",
  "TimeLimit",
  "FeasibilityTol",
  "IntFeasTol",
  "MarkowitzTol",
  "MIPGap",
  "MIPGapAbs",
  "OptimalityTol",
  "PerturbValue",
  "Heuristics",
  "ObjScale",
  "NodefileStart",
  "BarConvTol",
  "BarQCPConvTol",
  "PSDTol",
  "ImproveStartGap",
  "ImproveStartNodes",
  "ImproveStartTime",
  "FeasRelaxBigM",
  "TuneTimeLimit",
  "PreSOS1BigM",
  "PreSOS2BigM"
};

static const char* iparname[] = {
  "SolutionLimit",
  "Method",
  "ScaleFlag",
  "SimplexPricing",
  "Quad",
  "NormAdjust",
  "Sifting",
  "SiftMethod",
  "SubMIPNodes",
  "VarBranch",
  "Cuts",
  "CliqueCuts",
  "CoverCuts",
  "FlowCoverCuts",
  "FlowPathCuts",
  "GUBCoverCuts",
  "ImpliedCuts",
  "MIPSepCuts",
  "MIRCuts",
  "ModKCuts",
  "ZeroHalfCuts",
  "NetworkCuts",
  "SubMIPCuts",
  "CutAggPasses",
  "CutPasses",
  "GomoryPasses",
  "NodeMethod",
  "Presolve",
  "Aggregate",
  "IISMethod",
  "PreCrush",
  "PreDepRow",
  "PrePasses",
  "DisplayInterval",
  "OutputFlag",
  "Threads",
  "BarIterLimit",
  "Crossover",
  "CrossoverBasis",
  "BarCorrectors",
  "BarOrder",
  "PumpPasses",
  "RINS",
  "Symmetry",
  "MIPFocus",
  "NumericFocus",
  "AggFill",
  "PreDual",
  "SolutionNumber",
  "MinRelNodes",
  "ZeroObjNodes",
  "BranchDir",
  "InfUnbdInfo",
  "DualReductions",
  "BarHomogeneous",
  "PreQLinearize",
  "MIQCPMethod",
  "QCPDual",
  "LogToConsole",
  "PreSparsify",
  "PreMIQCPForm",
  "Seed",
  "ConcurrentMIP",
  "ConcurrentJobs",
  "DistributedMIPJobs",
  "LazyConstraints",
  "TuneResults",
  "TuneTrials",
  "TuneOutput",
  "TuneJobs",
  "Disconnected",
  "NoRelHeuristic",
  "UpdateMode",
  "WorkerPort",
  "Record"
};

static const char* sparname[] = {
  "LogFile",
  "NodefileDir",
  "ResultFile",
  "WorkerPool",
  "WorkerPassword",
  "Dummy"
};

#endif