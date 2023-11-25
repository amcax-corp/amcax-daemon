/* ===================================================================
* Copyright (C) 2023 Hefei Jiushao Intelligent Technology Co., Ltd. 
* All rights reserved.
*
* This software is licensed under the GNU Affero General Public License 
* v3.0 (AGPLv3.0) or a commercial license. You may choose to use this 
* software under the terms of either license.
*
* For more information about the AGPLv3.0 license, please visit: 
* https://www.gnu.org/licenses/agpl-3.0.html
* For licensing inquiries or to obtain a commercial license, please 
* contact Hefei Jiushao Intelligent Technology Co., Ltd.
* ===================================================================
* Author: 
*/
#include <deque>

#include <utility>
#include <string>
#include <topology/TopoShape.hpp>

namespace acamcad
{
	enum class ActionType {
		AddObject,
		DeleteObject, 
		OperateObject
	};

	class BaseObject; 
	class DataManager;

	class ContextHolder {
	public: 
		ContextHolder(int opId);
		ContextHolder(int opId, BaseObject* object, ActionType op_type);
		~ContextHolder(); 
	public :
		int getOperationId() { return operationId_; }
		int getObjectPid() { return objectPid_; }
		const AMCAX::TopoShape& getBackShape() { return backshape_; }
	private: 
		int operationId_; 
		int objectPid_;			// the persistentId of the object in datamanager; used to query whether the object still exists in data manager. 	
		AMCAX::TopoShape backshape_;	// the backup shape data for the object; 
	};

	typedef std::pair<ActionType, ContextHolder*> OperateContext;

	class UndoRedoHandler {
	public:
		UndoRedoHandler(DataManager* dataManager, int max_queue_size);
		~UndoRedoHandler(); 

	public: 
		void increaseSequence();	//
		void recordOperation(BaseObject* object, ActionType opType, bool incSeq = true);

		void undo(); 
		void redo(); 
		bool canUndo();
		bool canRedo();

	private: 
		void revokeOp(const OperateContext& context);
		void applyOp(const OperateContext& context);

	private: 
		std::deque<OperateContext> undo_queue_;
		std::deque<OperateContext> redo_queue_;
		void releaseQueueItem(OperateContext& context); 

	private: 
		int max_queue_size_;
		int operation_seq_;		//operations that share the same sequence 
		DataManager* dataManager_;
	};
}