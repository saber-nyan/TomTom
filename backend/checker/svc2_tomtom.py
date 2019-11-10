# -*- coding: utf-8 -*-
__author__ = 'saber-nyan'

import logging
import os
import random
import string
from os.path import join, dirname

import requests
import flatbuffers
from markovipy import MarkoviPy
import names
from random_username.generate import generate_username
from datetime import datetime
from redis import StrictRedis
from game_svc.config.game_settings import SERVICE_STATE_UP, SERVICE_STATE_MUMBLE, SERVICE_STATE_DOWN, \
    SERVICE_STATE_CORRUPTED
from game_svc.utils.other import get_redis_statuses_unique_key

from .svc2_flatbuffers.TomTomBackend.Request import Register as req_Reg, Message as req_Msg, \
    MessageType as req_MsgType, RequestContract as req_ReqContract, ListContracts as req_ListContracts, \
    CheckContract as req_CheckContract, Login as req_Log, GetMe as req_GetMe, EditMe as req_Ed
from .svc2_flatbuffers.TomTomBackend.Response import Message as res_Msg, Register as res_Reg, \
    RequestContract as res_ReqContract, Login as res_Log, ListContracts as res_ListContracts, \
    CheckContract as res_CheckContract, GetMe as res_GetMe, EditMe as res_Ed

from urllib3.exceptions import InsecureRequestWarning

PORT = '228'
SERVICE_NAME = 2
logger = logging.getLogger('checkers')
logger.propagate = False


class InvalidResponse(Exception):
    pass


def svc2_checker(team_id, ip, flag, redis_conf):
    logger.info('Starting svc2_checker')
    requests.packages.urllib3.disable_warnings(category=InsecureRequestWarning)

    redis_conn = StrictRedis(**redis_conf, charset="utf-8", decode_responses=True)

    # getting state from last run
    state_key = get_redis_statuses_unique_key(team_id=team_id, service_id=SERVICE_NAME)
    state = redis_conn.hgetall(state_key)

    last_flag = state.get("last_flag")
    flag_pushed = state.get("flag_pushed") == 'True'
    status = state.get("status")
    username = state.get("username")
    password = state.get("password")

    # PUSH if flag is new (new round) or we didn't pushed in last try
    if last_flag != flag or not flag_pushed:
        username = generate_username(1)[0]
        password = generate_str(16)
        status, trace = push_flag(ip, flag, username, password)
        flag_pushed = status == SERVICE_STATE_UP

    # try pull if flag is pushed
    if flag_pushed:
        status, trace = pull_flag(ip, flag, username, password)

    # if PUSH is OK
    if status == SERVICE_STATE_UP:
        status, trace = check_functionality(ip)

    # state for checker, write whatever need. No strict format
    service_status = {
        'date': datetime.now().isoformat(),
        'last_flag': flag,
        'flag_pushed': flag_pushed,
        'username': username,
        'password': password
    }

    # saving full state
    redis_conn.hmset(state_key, service_status)

    # state for scoreboard, strict properties
    redis_conn.hset(state_key, 'status', status)
    redis_conn.hset(state_key, 'trace', trace)

    logger.info('Finishing svc2_checker')

    # return status for SLA calculation
    return status


def generate_str(length=10):
    """Generate a random string of fixed length."""
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for _ in range(length))


def push_flag(ip, flag, username, password):
    first_name = names.get_first_name()
    last_name = names.get_last_name()
    # username = generate_username(1)[0]
    # password = generate_str(16)

    trace = 'push_flag ok'
    status = SERVICE_STATE_UP
    url = 'https://' + ip + ':' + PORT + '/api'
    print('-=-=-=-=-= Pushing flag to service w/ url: {} =-=-=-=-=-'.format(url))
    session = requests.Session()
    session.verify = False

    try:
        token = str(register(session, url, username, password, first_name, last_name), encoding='utf-8')
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot register: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot register.'
        return status, trace

    dataset_path = join(dirname(__file__), 'svc2_dataset.txt')
    obj = MarkoviPy(dataset_path)
    name = obj.generate_sentence()[:50]
    price = random.uniform(100, 37000000)
    attach = os.urandom(random.randrange(16, 8192))

    try:
        contract_id = request_contract(session, url, token, name, flag, price, attach)
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot create contract: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot create contract.'
        return status, trace

    print('Done pushing!')
    return status, trace


def pull_flag(ip, flag, username, password):
    trace = 'pull_flag ok'
    status = SERVICE_STATE_UP
    url = 'https://' + ip + ':' + PORT + '/api'
    print('-=-=-=-=-= Pulling flag from service w/ url: {} =-=-=-=-=-'.format(url))
    session = requests.Session()
    session.verify = False

    try:
        token = str(login(session, url, username, password), encoding='utf-8')
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot login: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot login.'
        return status, trace

    try:
        contracts_list = list_contracts(session, url, token)
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot list contracts: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot list contracts.'
        return status, trace

    flag_presents = False
    for i in range(contracts_list.ContractsLength()):
        res = contracts_list.Contracts(i)
        if flag in str(res.Description(), encoding='utf-8'):
            flag_presents = True
            break

    if not flag_presents:
        status = SERVICE_STATE_CORRUPTED
        trace = 'Flag not found.'
        return status, trace

    print('Done pulling!')
    return status, trace


def check_functionality(ip):
    first_name = names.get_first_name()
    new_first_name = names.get_first_name()
    last_name = names.get_last_name()
    username = generate_username(1)[0]
    password = generate_str(16)
    new_password = generate_str(16)

    trace = 'check_functionality ok'
    status = SERVICE_STATE_UP
    url = 'https://' + ip + ':' + PORT + '/api'
    print('-=-=-=-=-= Checking service w/ url: {} =-=-=-=-=-'.format(url))
    session = requests.Session()
    session.verify = False

    try:
        token = str(register(session, url, username, password, first_name, last_name), encoding='utf-8')
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot register: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot register.'
        return status, trace

    try:
        login_token = str(login(session, url, username, password), encoding='utf-8')
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot login: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot login.'
        return status, trace

    if token != login_token:
        status = SERVICE_STATE_MUMBLE
        trace = 'Token does not matches.'
        return status, trace

    try:
        u_name, f_name, l_name = get_me(session, url, token)
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot get profile info: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot get profile info.'
        return status, trace

    if u_name != username or f_name != first_name or l_name != last_name:
        print(f'DEBUG: was {u_name}, now {username}; was {f_name}, now {first_name}, was {l_name}, now {last_name}')
        status = SERVICE_STATE_MUMBLE
        trace = 'Profile info unexpectedly changed.'
        return status, trace

    try:
        new_token = str(edit_me(session, url, token, None, new_password, new_first_name, None), encoding='utf-8')
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot edit profile info: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot edit profile info.'
        return status, trace

    if new_token == login_token:
        status = SERVICE_STATE_MUMBLE
        trace = 'Token is not updated.'
        return status, trace

    dataset_path = join(dirname(__file__), 'svc2_dataset.txt')
    obj = MarkoviPy(dataset_path)
    name = obj.generate_sentence()[:50]
    description = obj.generate_sentence()
    price = random.uniform(100, 37000000)
    attach = os.urandom(random.randrange(16, 8192))

    try:
        contract_id = request_contract(session, url, new_token, name, description, price, attach)
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot create contract: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot create contract.'
        return status, trace

    try:
        contracts_list = list_contracts(session, url, new_token)
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot list contracts: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot list contracts.'
        return status, trace

    our_contract = None
    for i in range(contracts_list.ContractsLength()):
        res = contracts_list.Contracts(i)
        if res.Id() == contract_id:
            our_contract = res
            break

    if our_contract is None:
        status = SERVICE_STATE_MUMBLE
        trace = 'Created contract not found.'
        return status, trace

    user = our_contract.ByUser()
    if str(our_contract.Name(), encoding='utf-8') != name \
            or str(our_contract.Description(), encoding='utf-8') != description \
            or our_contract.AttachmentLength() != len(attach) \
            or str(user.Username(), encoding='utf-8') != username \
            or str(user.FirstName(), encoding='utf-8') != new_first_name \
            or str(user.LastName(), encoding='utf-8') != last_name:
        status = SERVICE_STATE_MUMBLE
        trace = 'Created contract has changed (1).'
        return status, trace

    try:
        our_contract = check_contract(session, url, new_token, contract_id)
    except InvalidResponse as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_MUMBLE
        trace = 'Cannot get contract: invalid response.'
        return status, trace
    except Exception as e:
        print('With exception: {}'.format(e))
        status = SERVICE_STATE_DOWN
        trace = 'Cannot get contract.'
        return status, trace

    user = our_contract.ByUser()
    if str(our_contract.Name(), encoding='utf-8') != name \
            or str(our_contract.Description(), encoding='utf-8') != description \
            or our_contract.AttachmentLength() != len(attach) \
            or str(user.Username(), encoding='utf-8') != username \
            or str(user.FirstName(), encoding='utf-8') != new_first_name \
            or str(user.LastName(), encoding='utf-8') != last_name:
        status = SERVICE_STATE_MUMBLE
        trace = 'Created contract has changed (2).'
        return status, trace

    print('Done checking!')
    return status, trace


def register(s, url, username, password, first_name, last_name):
    b = flatbuffers.Builder(0)
    username_b = b.CreateString(username)
    password_b = b.CreateString(password)
    first_name_b = b.CreateString(first_name)
    last_name_b = b.CreateString(last_name)

    req_Reg.RegisterStart(b)
    req_Reg.RegisterAddUsername(b, username_b)
    req_Reg.RegisterAddPassword(b, password_b)
    req_Reg.RegisterAddFirstName(b, first_name_b)
    req_Reg.RegisterAddLastName(b, last_name_b)
    table = req_Reg.RegisterEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().Register)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()
    #    print(f'Error? {is_error}\nDescription: {error_description}')

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    union_table = res_Reg.Register()
    union_table.Init(msg.Bytes, msg.Pos)

    return union_table.Token()


def login(s, url, username, password):
    b = flatbuffers.Builder(0)
    username_b = b.CreateString(username)
    password_b = b.CreateString(password)

    req_Log.LoginStart(b)
    req_Log.LoginAddUsername(b, username_b)
    req_Log.LoginAddPassword(b, password_b)
    table = req_Reg.RegisterEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().Login)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()
    #    print(f'Error? {is_error}\nDescription: {error_description}')

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    union_table = res_Log.Login()
    union_table.Init(msg.Bytes, msg.Pos)

    return union_table.Token()


def get_me(s, url, token):
    b = flatbuffers.Builder(0)
    token_b = b.CreateString(token)

    req_GetMe.GetMeStart(b)
    req_GetMe.GetMeAddToken(b, token_b)
    table = req_GetMe.GetMeEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().GetMe)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()
    #    print(f'Error? {is_error}\nDescription: {error_description}')

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    r = res_GetMe.GetMe()
    r.Init(msg.Bytes, msg.Pos)

    return str(r.Username(), encoding='utf-8'), \
           str(r.FirstName(), encoding='utf-8'), str(r.LastName(), encoding='utf-8')


def edit_me(s, url, token,
            username,
            password,
            first_name,
            last_name
            ):
    b = flatbuffers.Builder(0)
    token_b = b.CreateString(token)
    username_b = None
    if username is not None:
        username_b = b.CreateString(username)
    password_b = b.CreateString(password)
    first_name_b = b.CreateString(first_name)
    last_name_b = None
    if last_name is not None:
        last_name_b = b.CreateString(last_name)

    req_Ed.EditMeStart(b)
    req_Ed.EditMeAddToken(b, token_b)
    if username_b is not None:
        req_Ed.EditMeAddUsername(b, username_b)
    req_Ed.EditMeAddPassword(b, password_b)
    req_Ed.EditMeAddFirstName(b, first_name_b)
    if last_name_b is not None:
        req_Ed.EditMeAddLastName(b, last_name_b)
    table = req_Ed.EditMeEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().EditMe)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()
    #    print(f'Error? {is_error}\nDescription: {error_description}')

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    r = res_Ed.EditMe()
    r.Init(msg.Bytes, msg.Pos)

    return r.Token()


def request_contract(s, url, token: str, name: str, description, price: float, attach) -> int:
    b = flatbuffers.Builder(0)
    token_b = b.CreateString(token)
    name_b = b.CreateString(name)
    if description:
        description_b = b.CreateString(description)

    attach_b = b.CreateByteVector(attach)

    req_ReqContract.RequestContractStart(b)
    req_ReqContract.RequestContractAddToken(b, token_b)
    req_ReqContract.RequestContractAddName(b, name_b)
    if description:
        # noinspection PyUnboundLocalVariable
        req_ReqContract.RequestContractAddDescription(b, description_b)
    req_ReqContract.RequestContractAddPrice(b, price)
    req_ReqContract.RequestContractAddAttachment(b, attach_b)
    table = req_ReqContract.RequestContractEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().RequestContract)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    union_table = res_ReqContract.RequestContract()
    union_table.Init(msg.Bytes, msg.Pos)

    res = union_table.Result()

    return res.Id()


def list_contracts(s, url, token: str):
    b = flatbuffers.Builder(0)
    token_b = b.CreateString(token)

    req_ListContracts.ListContractsStart(b)
    req_ListContracts.ListContractsAddToken(b, token_b)
    table = req_ListContracts.ListContractsEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().ListContracts)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    union_table = res_ListContracts.ListContracts()
    union_table.Init(msg.Bytes, msg.Pos)

    return union_table


def check_contract(s, url, token: str, _id: int):
    b = flatbuffers.Builder(0)
    token_b = b.CreateString(token)

    req_CheckContract.CheckContractStart(b)
    req_CheckContract.CheckContractAddToken(b, token_b)
    req_CheckContract.CheckContractAddId(b, _id)
    table = req_CheckContract.CheckContractEnd(b)

    req_Msg.MessageStart(b)
    req_Msg.MessageAddMsgType(b, req_MsgType.MessageType().CheckContract)
    req_Msg.MessageAddMsg(b, table)
    r = req_Msg.MessageEnd(b)

    b.Finish(r)
    binary = b.Output()

    response = s.post(url, binary, timeout=5, allow_redirects=False).content

    parsed_response = res_Msg.Message.GetRootAsMessage(response, 0)
    is_error = parsed_response.Error()
    error_description = parsed_response.ErrorDescription()

    msg = parsed_response.Msg()
    if not msg or is_error:
        raise InvalidResponse(error_description)
    union_table = res_CheckContract.CheckContract()
    union_table.Init(msg.Bytes, msg.Pos)

    return union_table
