﻿/*
	Copyright (c) 2017 TOSHIBA Digital Solutions Corporation

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
	@file
	@brief Definition of MessageSchema
*/
#ifndef MESSAGE_SCHEMA_H_
#define MESSAGE_SCHEMA_H_

#include "collection.h"
#include "time_series.h"
#include "value_operator.h"

/*!
	@brief Case intensive compare method for string
*/
struct CompareStringI {
public:
	bool operator()(const util::String &right, const util::String &left) const {
		return (compareStringStringI(
					right.c_str(),
					static_cast<uint32_t>(right.length()),
					left.c_str(),
					static_cast<uint32_t>(left.length())) < 0);
	}
};

/*!
	@brief Container schema for message format
*/
class MessageSchema {
public:
	MessageSchema(util::StackAllocator &alloc,
		const DataStoreValueLimitConfig &dsValueLimitConfig,
		const char *containerName, util::ArrayByteInStream &in);
	virtual ~MessageSchema() {}


	ContainerType getContainerType() const {
		return containerType_;
	}


	uint32_t getColumnCount() const {
		return columnNum_;
	}

	const util::XArray<ColumnId>& getRowKeyColumnIdList() const {
		return keyColumnIds_;
	}
	uint32_t getRowKeyNum() const {
		return static_cast<uint32_t>(keyColumnIds_.size());
	}

	ColumnType getColumnType(ColumnId columnId) const {
		return columnTypeList_[columnId];
	}

	ColumnType getColumnFullType(ColumnId columnId) const;

	bool getIsArray(ColumnId columnId) const {
		return (flagsList_[columnId] & COLUMN_FLAG_ARRAY) != 0;
	}
	bool getIsNotNull(ColumnId columnId) const {
		return (flagsList_[columnId] & COLUMN_FLAG_NOT_NULL) != 0;
	}

	bool getIsVirtual(ColumnId columnId) const {
		return (flagsList_[columnId] & COLUMN_FLAG_VIRTUAL) != 0;
	}

	const util::String &getColumnName(ColumnId columnId) const {
		return columnNameList_[columnId];
	}

	const util::String &getAffinityStr() const {
		return affinityStr_;
	}

	ContainerAttribute getContainerAttribute() const {
		return containerAttribute_;
	}


	void setAffinityStr(const char *affinity) {
		affinityStr_ = affinity;
	}

protected:
	void validateColumnSchema(util::ArrayByteInStream &in);
	void validateContainerOption(util::ArrayByteInStream &in);

	util::StackAllocator &getAllocator() {
		return alloc_;
	}
	void setColumnCount(uint32_t count);

	void setRowKeyColumnId(ColumnId columnId);

	void setColumnType(ColumnId columnId, ColumnType type);

	void setColumnName(ColumnId columnId, const void *data, uint32_t size);

	void setContainerAttribute(ContainerAttribute attribute) {
		containerAttribute_ = attribute;
	}

protected:
	const DataStoreValueLimitConfig &dsValueLimitConfig_;
	ContainerType containerType_;
	util::String affinityStr_;  
private:
	static const uint8_t COLUMN_FLAG_ARRAY = 0x01;
	static const uint8_t COLUMN_FLAG_VIRTUAL = 0x02;
	static const uint8_t COLUMN_FLAG_NOT_NULL = 0x04;

	uint32_t columnNum_;
	util::XArray<ColumnId> keyColumnIds_;
	util::XArray<ColumnType> columnTypeList_;
	util::XArray<uint8_t> flagsList_;
	util::Vector<util::String> columnNameList_;
	util::Map<util::String, ColumnId, CompareStringI> columnNameMap_;
	util::StackAllocator &alloc_;
	ContainerAttribute containerAttribute_;
};

/*!
	@brief Collection schema for message format
*/
class MessageCollectionSchema : public MessageSchema {
public:
	MessageCollectionSchema(util::StackAllocator &alloc,
		const DataStoreValueLimitConfig &dsValueLimitConfig,
		const char *containerName, util::ArrayByteInStream &in);

	~MessageCollectionSchema() {}
protected:
};

/*!
	@brief TimeSeries schema for message format
*/
class MessageTimeSeriesSchema : public MessageSchema {
public:
	MessageTimeSeriesSchema(util::StackAllocator &alloc,
		const DataStoreValueLimitConfig &dsValueLimitConfig,
		const char *containerName, util::ArrayByteInStream &in);

	~MessageTimeSeriesSchema() {}

	TimeSeries::ExpirationInfo &getExpirationInfo() {
		return expirationInfo_;
	}

	bool isExistTimeSeriesOption() const {
		return isExistTimeSeriesOption_;
	}


	void setExpirationInfo(const TimeSeries::ExpirationInfo &expirationInfo) {
		expirationInfo_ = expirationInfo;
	}

protected:
private:
	void validateRowKeySchema();
	void validateOption(util::ArrayByteInStream &in);

private:
	TimeSeries::ExpirationInfo expirationInfo_;  
	bool isExistTimeSeriesOption_;
};

#endif
