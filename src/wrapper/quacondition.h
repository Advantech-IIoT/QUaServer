#ifndef QUACONDITION_H
#define QUACONDITION_H

#include <QUaBaseEvent>

#ifdef UA_ENABLE_SUBSCRIPTIONS_ALARMS_CONDITIONS

class QUaTwoStateVariable;
class QUaConditionVariable;
class QUaConditionBranch;

class QUaCondition : public QUaBaseEvent
{
    Q_OBJECT

	// NOTE : any property that has special logic should be made a writable metaproperty to deserialize properly
	Q_PROPERTY(QString sourceNode READ sourceNode WRITE setSourceNode)
	Q_PROPERTY(bool    retain     READ retain     WRITE setRetain    )
	Q_PROPERTY(quint16 severity   READ severity   WRITE setSeverity  )

friend class QUaServer;
friend class QUaConditionBranch;

public:
	Q_INVOKABLE explicit QUaCondition(
		QUaServer *server
	);
	~QUaCondition();

	// inherited

	// Overwrite QUaNode::setDisplayName to update conditionName
	virtual void setDisplayName(const QUaLocalizedText& displayName) override;

	// Overwrite QUaBaseEvent::setSourceNode to update retained conditions cache
	virtual void setSourceNode(const QUaNodeId& sourceNodeId) override;

	// children

	// TODO : implement condition classes

	QUaNodeId conditionClassId() const;
	void      setConditionClassId(const QUaNodeId& conditionClassId);

	QUaLocalizedText conditionClassName() const;
	void    setConditionClassName(const QUaLocalizedText& conditionClassName);

	// NOTE : optional; not created until one of these methods is called
	QList<QUaNodeId> conditionSubClassId() const;
	void      setConditionSubClassId(const QList<QUaNodeId>& conditionSubClassId);

	// ConditionSubClassName provides the display name[s] of the ConditionClassType[s] listed in
	// the ConditionSubClassId.
	// NOTE : optional; not created until one of these methods is called
	QList<QUaLocalizedText> conditionSubClassName() const;
	void    setConditionSubClassName(const QList<QUaLocalizedText>& conditionSubClassName);

	// to distinguish from which condition the event originated from
	QString conditionName() const;
	void    setConditionName(const QString& conditionName);

	// only set if it is a branch, null if not a branch
	QUaNodeId branchId() const;
	void      setBranchId(const QUaNodeId& branchId);

	// retain is used to let know client that a condition if of interest at
	// the current time, and for condition sync
	bool    retain() const;
	void    setRetain(const bool& retain);

	QUaLocalizedText enabledStateCurrentStateName() const;
	void             setEnabledStateCurrentStateName(const QUaLocalizedText& enabledState);
	bool             enabledStateId() const;
	void             setEnabledStateId(const bool& enabledStateId);
	QDateTime        enabledStateTransitionTime() const;
	void             setEnabledStateTransitionTime(const QDateTime& transitionTime);
	QUaLocalizedText enabledStateTrueState() const;
	void             setEnabledStateTrueState(const QUaLocalizedText& trueState);
	QUaLocalizedText enabledStateFalseState() const;
	void             setEnabledStateFalseState(const QUaLocalizedText& falseState);
	// helper sets EnabledStateId, EnabledStateCurrentStateName, EnabledStateTransitionTime 
	// and triggers event, also handles retain and branches according to specification
	// NOTE : change of the Enabled state must be normally make by the client through
	//        the use of the Enable() and Disable() methods
	bool      enabled() const;
	void      setEnabled(const bool& enabled);

	// quality of underlying source
	QUaStatusCode quality() const;
	void          setQuality(const QUaStatusCode& quality);

	// 
	quint16 lastSeverity() const;
	void setLastSeverity(const quint16& lastSeverity);
	// Overwrite to handle lastSeverity automatically
	virtual void setSeverity(const quint16& intSeverity) override;

	QUaLocalizedText comment() const;
	void             setComment(const QUaLocalizedText& comment);

	// identity of who put the last comment
	QString clientUserId() const;
	void    setClientUserId(const QString& clientUserId);

	// methods

	Q_INVOKABLE void Enable(); // TODO : audit events

	Q_INVOKABLE void Disable(); // TODO : audit events

	Q_INVOKABLE void AddComment(QByteArray EventId, QUaLocalizedText Comment); // TODO : audit events

	// branches API

	template<typename T>
	T* createBranch(const QUaNodeId& branchId = QUaNodeId());

	// get all branches
	QList<QUaConditionBranch*> branches() const;

	template<typename T>
	QList<T*> branches() const;

	bool hasBranches() const;

	// get branch by EventId (OPC UA Methods can be called by EventId)
	QUaConditionBranch* branchByEventId(const QByteArray& eventId) const;

	template<typename T>
	T* branchByEventId(const QByteArray& EventId) const;

	// remove from list only, do not delete
	void removeBranchByEventId(QUaConditionBranch* branch);

signals:
	void conditionEnabled();

	void conditionDisabled();

	void addedComment(const QString& comment);

	// TODO : branchCreated

protected:
	// NodeId
	QUaProperty* getConditionClassId();
	// LocalizedText
	QUaProperty* getConditionClassName();
	// NodeId[]
	QUaProperty* getConditionSubClassId();
	// LocalizedText[]
	QUaProperty* getConditionSubClassName();
	// String
	QUaProperty* getConditionName();
	// NodeId
	QUaProperty* getBranchId();
	// Boolean
	QUaProperty* getRetain();
	// LocalizedText
	QUaTwoStateVariable* getEnabledState();
	// StatusCode
	// NOTE : any change on a ConditionVariable should trigger
	QUaConditionVariable* getQuality();
	// UInt16
	// NOTE : any change on a ConditionVariable should trigger
	QUaConditionVariable* getLastSeverity();
	// LocalizedText
	// NOTE : any change on a ConditionVariable should trigger
	QUaConditionVariable* getComment();
	// String
	QUaProperty* getClientUserId();

	// helpers

	// reimplement to define minimu trigger conditions
	virtual bool shouldTrigger() const override;
	// reimplement to define retain and branch creation
	virtual bool requiresAttention() const;
	// reimplement to reset type internals (QUaAlarmCondition::Reset)
	virtual void resetInternals();

private:
	QUaNode * m_sourceNode;
	QMetaObject::Connection m_sourceDestroyed;
	QMetaObject::Connection m_retainedDestroyed;

	QList<QUaConditionBranch*> m_branches;

	//
	static UA_StatusCode ConditionRefresh(
		UA_Server*        server,
		const UA_NodeId*  sessionId,
		void*             sessionContext,
		const UA_NodeId*  methodId,
		void*             methodContext,
		const UA_NodeId*  objectId,
		void*             objectContext,
		size_t            inputSize,
		const UA_Variant* input,
		size_t            outputSize,
		UA_Variant*       output
	);
	//
	static UA_StatusCode ConditionRefresh2(
		UA_Server*        server,
		const UA_NodeId*  sessionId,
		void*             sessionContext,
		const UA_NodeId*  methodId,
		void*             methodContext,
		const UA_NodeId*  objectId,
		void*             objectContext,
		size_t            inputSize,
		const UA_Variant* input,
		size_t            outputSize,
		UA_Variant*       output
	);

	// helpers
	static void processMonitoredItem(
		UA_MonitoredItem* monitoredItem,
		QUaServer* svr
	);

};

template<typename T>
inline T* QUaCondition::createBranch(const QUaNodeId& branchId/* = ""*/)
{
	auto branch = new T(this, branchId);
	m_branches << branch;
	return branch;
}

template<typename T>
inline QList<T*> QUaCondition::branches() const
{
	QList<T*> retBranches;
	for (auto branch : m_branches)
	{
		auto specialized = qobject_cast<T*>(branch);
		if (!specialized)
		{
			continue;
		}
		retBranches << specialized;
	}
	return retBranches;
}

template<typename T>
inline T* QUaCondition::branchByEventId(const QByteArray& EventId) const
{
	return dynamic_cast<T*>(this->branchByEventId(EventId));
}

// NOTE : to support branches, a simplified copy of the condition at a certain
// point in time is stored in the QUaConditionBranch class.
// This class provides mechanisms for triggering events for the branch and 
// accessing and changing the values stored in the branch.
// The derived classes of QUaConditionBranch are only meant to provide syntactic
// sugar around the ::value and ::setValue methods to access relevant fields.
// For example, the QUaAcknowledgeableConditionBranch derived class should provide
// helper methods to change the Acked or Confirmed state of the branch: All these
// helper methods will use the underlying ::value and ::setValue methods anyways
class QUaConditionBranch
{
public:
	QUaConditionBranch(
		QUaCondition* parent, 
		const QUaNodeId& branchId = QUaNodeId()
	);
	~QUaConditionBranch();

	void deleteLater();

	// Common API

	QVariant value(const QList<QUaQualifiedName>& browsePath) const;
	void     setValue(const QList<QUaQualifiedName>& browsePath, const QVariant& value);

	// Event specific API

	void trigger();

	QByteArray eventId() const;
	void setEventId(const QByteArray& eventId);

	void setMessage(const QUaLocalizedText& message);

	void setTime(const QDateTime& time);

	// Event specific API

	QUaNodeId branchId() const;
	void setBranchId(const QUaNodeId& branchId);

	void setRetain(const bool& retain);

	void setComment(
		const QUaLocalizedText& comment, 
		const QString& currentUser = QString()
	);

protected:
	QUaCondition* m_parent;

	struct QUaBranchNode
	{
		inline QVariant value() const
		{
			return m_isVariable ? m_value : QVariant::fromValue(m_nodeId);
		}
		inline void setValue(const QVariant& value)
		{
			m_isVariable ? m_value = value : m_nodeId = value.value<QUaNodeId>();
		}

		bool m_isVariable : 1;
		QUaNodeId m_nodeId;
		QVariant  m_value;
		QHash<QUaQualifiedName, QUaBranchNode> m_children;
	};

	QUaBranchNode m_root;

	void addChildren(QUaNode* node, QUaBranchNode& branchNode);

	static QList<QUaQualifiedName> saoToBrowsePath(const UA_SimpleAttributeOperand* sao);

	// QUaBaseEvent
	static QList<QUaQualifiedName> EventId;
	static QList<QUaQualifiedName> Message;
	static QList<QUaQualifiedName> Time;
	static QList<QUaQualifiedName> ClientUserId;
	// QUaCondition
	static QList<QUaQualifiedName> BranchId;
	static QList<QUaQualifiedName> Retain;
	static QList<QUaQualifiedName> EnabledState;
	static QList<QUaQualifiedName> EnabledState_Id;
	static QList<QUaQualifiedName> EnabledState_FalseState;
	static QList<QUaQualifiedName> EnabledState_TrueState;
	static QList<QUaQualifiedName> EnabledState_TransitionTime;
	static QList<QUaQualifiedName> Comment;
	static QList<QUaQualifiedName> Comment_SourceTimestamp;
	// setClientUserId

	// reimplement to define retain and branch creation
	virtual bool requiresAttention() const;

	friend class QUaCondition;
	friend class QUaServer_Anex;
};

#endif // UA_ENABLE_SUBSCRIPTIONS_ALARMS_CONDITIONS

#endif // QUACONDITION_H
