#include <cstddef>

/**
 * Copyright (C) 2021, Stealth Software Technologies, Inc.
 */

#include <string>
#include <chrono>

#include <wtk/Parser.h>
#include <wtk/IRTree.h>
#include <wtk/IRParameters.h>
#include <wtk/irregular/Parser.h>
#include <wtk/firealarm/TreeAlarm.h>

#include <wtk/utils/IRTreeUtils.h>

#include <wtk/bolt/Builder.h>
#include <wtk/bolt/Evaluator.h>
#include <wtk/bolt/Backend.h>
#include <wtk/bolt/PLASMASnooze.h>
#include <wtk/bolt/ArithmeticPLASMASnoozeHandler.h>
#include <wtk/bolt/BooleanPLASMASnoozeHandler.h>

#define LOG_IDENTIFIER "emp-interpret"
#include <stealth_logging.h>

#include "emp-ir/emp_arith_backend.h"
#include "emp-ir/emp_bool_backend.h"

template<typename Wire_T, typename Number_T>
int do_bolt(wtk::IRTree<Number_T>* rel_tree,
    wtk::InputStream<Number_T>* ins_stream,
    wtk::InputStream<Number_T>* wit_stream,
    wtk::bolt::Backend<Wire_T, Number_T>* backend)
{
  int ret = 0;
  wtk::bolt::Builder<Wire_T, Number_T> builder(backend->prime);

  wtk::bolt::Bolt<Wire_T, Number_T>* bolt = builder.build(rel_tree);

  if(bolt != nullptr)
  {
    wtk::bolt::Evaluator<Wire_T, Number_T> eval(backend);

    if(!eval.evaluate(bolt, ins_stream, wit_stream))
    {
      log_error("interpreter: instance/witnesss poorly formed");
      ret = 1;
    }
    else if(!backend->check())
    {
      log_error("backend: proof invalid");
      ret = 1;
    }
  }
  else
  {
    log_error("interpreter: relation poorly formed");
    ret = 1;
  }

  backend->finish();
  return ret;
}

template<typename Wire_T, typename Number_T>
int do_plasmasnooze(wtk::IRTree<Number_T>* rel_tree,
    wtk::InputStream<Number_T>* ins_stream,
    wtk::InputStream<Number_T>* wit_stream,
    wtk::bolt::Backend<Wire_T, Number_T>* backend)
{
  int ret = 0;

  wtk::bolt::PLASMASnooze<Wire_T, Number_T> snooze(backend);

  wtk::bolt::PLASMASnoozeStatus status =
    snooze.evaluate(rel_tree, ins_stream, wit_stream);
  if(wtk::bolt::PLASMASnoozeStatus::bad_relation == status)
  {
    log_error("interpreter: relation poorly formed");
    ret = 1;
  }
  else if(wtk::bolt::PLASMASnoozeStatus::bad_stream == status)
  {
    log_error("interpreter: instance/witnesss poorly formed");
    ret = 1;
  }
  else if(!backend->check())
  {
    log_error("backend: proof invalid");
    ret = 1;
  }
  backend->finish();

  return ret;
}

template<typename Parser_T, typename Wire_T, typename Number_T>
int do_stream(Parser_T* const relation,
    wtk::InputStream<Number_T>* const ins,
    wtk::InputStream<Number_T>* const wit,
    wtk::bolt::Backend<Wire_T, Number_T>* const backend);

template<>
int do_stream(wtk::ArithmeticParser<uint64_t>* const relation,
    wtk::InputStream<uint64_t>* const ins,
    wtk::InputStream<uint64_t>* const wit,
    wtk::bolt::Backend<__uint128_t, uint64_t>* const backend)
{
  wtk::bolt::ArithmeticPLASMASnoozeHandler<__uint128_t, uint64_t> handler(
      backend, ins, wit);

  int ret = 0;
  if(!relation->parseStream(&handler))
  {
    ret = 1;
  }
  else
  {
    wtk::bolt::PLASMASnoozeStatus status = handler.check();
    if(wtk::bolt::PLASMASnoozeStatus::bad_relation == status)
    {
      log_error("interpreter: relation poorly formed");
      ret = 1;
    }
    else if(wtk::bolt::PLASMASnoozeStatus::bad_stream == status)
    {
      log_error("interpreter: instance/witness poorly formed");
      ret = 1;
    }
    else if(!backend->check())
    {
      log_error("backend: proof invalid");
      ret = 1;
    }
    else
    {
      ret = 0;
    }
  }

  backend->finish();
  return ret;
}

template<>
int do_stream(wtk::BooleanParser* const relation,
    wtk::InputStream<uint8_t>* const ins,
    wtk::InputStream<uint8_t>* const wit,
    wtk::bolt::Backend<block, uint8_t>* const backend)
{
  wtk::bolt::BooleanPLASMASnoozeHandler<block> handler(backend, ins, wit);

  int ret = 0;
  if(!relation->parseStream(&handler))
  {
    ret = 1;
  }
  else
  {
    wtk::bolt::PLASMASnoozeStatus status = handler.check();
    if(wtk::bolt::PLASMASnoozeStatus::bad_relation == status)
    {
      log_error("interpreter: relation poorly formed");
      ret = 1;
    }
    else if(wtk::bolt::PLASMASnoozeStatus::bad_stream == status)
    {
      log_error("interpreter: instance/witness poorly formed");
      ret = 1;
    }
    else if(!backend->check())
    {
      log_error("backend: proof invalid");
      ret = 1;
    }
    else
    {
      ret = 0;
    }
  }

  backend->finish();
  return ret;
}

template<typename Parser_T, typename Wire_T, typename Number_T>
int do_delegation2(Parser_T* const relation,
    bool const stream, bool const plasmasnooze,
    wtk::InputStream<Number_T>* const ins,
    wtk::InputStream<Number_T>* const wit,
    wtk::bolt::Backend<Wire_T, Number_T>* const backend)
{
  if(stream)
  {
    return do_stream(relation, ins, wit, backend);
  }
  else
  {
    wtk::IRTree<Number_T>* rel_tree = relation->parseTree();
    if(rel_tree != nullptr)
    {
      if(plasmasnooze) { return do_plasmasnooze(rel_tree, ins, wit, backend); }
      else { return do_bolt(rel_tree, ins, wit, backend); }
    }
    else
    {
      return 1;
    }
  }
}

int do_delegation1(wtk::Parser<uint64_t>* const parser,
    bool const stream, bool const plasmasnooze,
    int const port, int const threads, int const party, char const* const ip,
    std::string& ins_str, std::string& wit_str)
{
  if(parser->gateSet.gateSet == wtk::GateSet::arithmetic)
  {
    if(party == ALICE)
    {
      wtk::irregular::Parser<uint64_t> ins_parser(ins_str);
      wtk::irregular::Parser<uint64_t> wit_parser(wit_str);

      if(!ins_parser.parseHdrResParams())
      {
        return 1;
      }
      else if(ins_parser.resource != wtk::Resource::instance)
      {
        log_error("%s: not an instance.", ins_str.c_str());
        return 1;
      }
      else if(ins_parser.characteristic != parser->characteristic)
      {
        log_error("%s: prime %" PRIu64 " does not match relation %" PRIu64,
            ins_str.c_str(), ins_parser.characteristic, parser->characteristic);
        return 1;
      }

      if(!wit_parser.parseHdrResParams())
      {
        return 1;
      }
      else if(wit_parser.resource != wtk::Resource::shortWitness)
      {
        log_error("%s: not a short witness.", wit_str.c_str());
        return 1;
      }
      else if(wit_parser.characteristic != parser->characteristic)
      {
        log_error("%s: prime %" PRIu64 " does not match relation %" PRIu64,
            wit_str.c_str(), wit_parser.characteristic, parser->characteristic);
        return 1;
      }

      EMPArithBackend backend(ip, port, party, threads);
      return do_delegation2(parser->arithmetic(), stream, plasmasnooze,
          ins_parser.arithmetic()->instance(),
          wit_parser.arithmetic()->shortWitness(), &backend);
    }
    else
    {
      wtk::irregular::Parser<uint64_t> ins_parser(ins_str);

      if(!ins_parser.parseHdrResParams())
      {
        return 1;
      }
      else if(ins_parser.resource != wtk::Resource::instance)
      {
        log_error("%s: not an instance.", ins_str.c_str());
        return 1;
      }
      else if(ins_parser.characteristic != parser->characteristic)
      {
        log_error("%s: prime %" PRIu64 " does not match relation %" PRIu64,
            ins_str.c_str(), ins_parser.characteristic, parser->characteristic);
        return 1;
      }

      EMPArithBackend backend(ip, port, party, threads);
      return do_delegation2(parser->arithmetic(), stream, plasmasnooze,
          ins_parser.arithmetic()->instance(),
          (wtk::InputStream<uint64_t>*) nullptr, &backend);
    }
  }
  else
  {
    if(party == ALICE)
    {
      wtk::irregular::Parser<uint8_t> ins_parser(ins_str);
      wtk::irregular::Parser<uint8_t> wit_parser(wit_str);

      if(!ins_parser.parseHdrResParams())
      {
        return 1;
      }
      else if(ins_parser.resource != wtk::Resource::instance)
      {
        log_error("%s: not an instance.", ins_str.c_str());
        return 1;
      }
      else if(ins_parser.characteristic != (uint8_t) parser->characteristic)
      {
        log_error("%s: prime %" PRIu8 " does not match relation %" PRIu64,
            ins_str.c_str(), ins_parser.characteristic, parser->characteristic);
        return 1;
      }

      if(!wit_parser.parseHdrResParams())
      {
        return 1;
      }
      else if(wit_parser.resource != wtk::Resource::shortWitness)
      {
        log_error("%s: not a short witness.", wit_str.c_str());
        return 1;
      }
      else if(wit_parser.characteristic != (uint8_t) parser->characteristic)
      {
        log_error("%s: prime %" PRIu8 " does not match relation %" PRIu64,
            wit_str.c_str(), wit_parser.characteristic, parser->characteristic);
        return 1;
      }

      EMPBoolBackend backend(ip, port, party, threads);
      return do_delegation2(parser->boolean(), stream, plasmasnooze,
          ins_parser.boolean()->instance(),
          wit_parser.boolean()->shortWitness(), &backend);
    }
    else
    {
      wtk::irregular::Parser<uint8_t> ins_parser(ins_str);

      if(!ins_parser.parseHdrResParams())
      {
        return 1;
      }
      else if(ins_parser.resource != wtk::Resource::instance)
      {
        log_error("%s: not an instance.", ins_str.c_str());
        return 1;
      }
      else if(ins_parser.characteristic != (uint8_t) parser->characteristic)
      {
        log_error("%s: prime %" PRIu8 " does not match relation %" PRIu64,
            ins_str.c_str(), ins_parser.characteristic, parser->characteristic);
        return 1;
      }

      EMPBoolBackend backend(ip, port, party, threads);
      return do_delegation2(parser->boolean(), stream, plasmasnooze,
          ins_parser.boolean()->instance(),
          (wtk::InputStream<uint8_t>*) nullptr, &backend);
    }
  }
}

int main(int argc, char const* argv[])
{
	if(argc != 6 && argc != 7 && argc != 8)  {
		log_error("USAGE: %s [mode?] [ip] [port] [threads] [relation] [instance] [witness?]", argv[0]);
    log_info("    Presence of witness indicates party");
    log_info("    Available modes:");
    log_info("      bolt            fast two pass interpreter (default)");
    log_info("      plasmasnoose    single pass interpreter");
    log_info("      stream          streaming parser/interpreter (IR-Simple only)");
    return 1;
	}

  bool use_plasmasnooze = false;
  bool use_streaming = false;
  size_t arg_place = 2;

  std::string evaluator_str(argv[1]);
  if(evaluator_str == "plasmasnooze") { use_plasmasnooze = true; }
  else if(evaluator_str == "stream") { use_streaming = true; }
  else if(evaluator_str != "bolt")
  {
    arg_place = 1;
  }

  char const* const ip_addr = argv[arg_place++];
  int port = atoi(argv[arg_place++]);
  int threads = atoi(argv[arg_place++]);
  int party;

  std::string rel_str(argv[arg_place++]);
  std::string ins_str(argv[arg_place++]);
  std::string wit_str;

  if(arg_place < (size_t) argc)
  {
    wit_str = argv[arg_place];
    party = ALICE;
  }
  else
  {
    party = BOB;
  }

  wtk::irregular::Parser<uint64_t> rel(rel_str);
  if(rel.parseHdrResParams() && rel.resource == wtk::Resource::relation)
  {
    if((rel.gateSet.gateSet == wtk::GateSet::arithmetic
          && rel.characteristic !=  (1ULL << 61) - 1)
        || (rel.gateSet.gateSet == wtk::GateSet::boolean
          && rel.characteristic != 2))
    {
      log_error("bad characteristic/prime: %" PRIu64, rel.characteristic);
      return 1;
    }

    return do_delegation1(&rel, use_streaming, use_plasmasnooze, port, threads,
        party, ip_addr, ins_str, wit_str);
  }

  return 1;
}
